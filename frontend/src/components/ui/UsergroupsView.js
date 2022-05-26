/*
 * This file is part of the Tsailun project 
 *
 * Copyright (c) 2021-2022 Li Supeng
 *
 * Tsailun is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * Tsailun is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tsailun.  If not, see <http://www.gnu.org/licenses/>.
 */

import React from "react";
import { useHistory } from "react-router-dom";
import Paper from "@material-ui/core/Paper";
import Table from "@material-ui/core/Table";
import TableBody from "@material-ui/core/TableBody";
import TableCell from "@material-ui/core/TableCell";
import TableContainer from "@material-ui/core/TableContainer";
import TableHead from "@material-ui/core/TableHead";
//import TablePagination from '@material-ui/core/TablePagination';
import TableRow from "@material-ui/core/TableRow";

import Button from "@material-ui/core/Button";

import IconButton from "@material-ui/core/IconButton";
import EditIcon from "@material-ui/icons/Edit";
import DeleteIcon from "@material-ui/icons/Delete";

import Dialog from "@material-ui/core/Dialog";
import DialogActions from "@material-ui/core/DialogActions";
import DialogContent from "@material-ui/core/DialogContent";
import DialogContentText from "@material-ui/core/DialogContentText";
import DialogTitle from "@material-ui/core/DialogTitle";

import TextField from "@material-ui/core/TextField";

import Typography from "@material-ui/core/Typography";

import Utils from "../Utils";

import Lang from "../../i18n/Lang";

import Box from "@material-ui/core/Box";

var usergroup = "";

const columns = [
  { id: "groupname", label: Lang.str_admin_group_name, minWidth: 120 },
  /*
  {
    id: 'usercount',
    label: 'User List',
    minWidth: 170,
    format: (value) => value.toLocaleString('en-US'),
  },
  */
  {
    id: "comment",
    label: Lang.str_admin_group_comment,
    minWidth: 80,
  },
  {
    id: "operate",
    label: "",
    minWidth: 170,
    align: "right",
  },
];

/*function createData(groupname, usercount, comment, operate) {
  return { groupname, usercount, comment, operate };
}*/

export default function UsergroupsView() {
  let history = useHistory();

  const [rows, setRows] = React.useState([]);
  const [groupName, setGroupName] = React.useState("");
  const [comment, setComment] = React.useState("");

  const [errmsg, setErrMsg] = React.useState("");

  const [delUserGroupDlgOpen, setDelUserGroupDlgOpen] = React.useState(false);
  const handleCloseDelUserGroupDlg = () => {
    setDelUserGroupDlgOpen(false);
  };

  const groupListRefresh = () => {
    var url_listusr = "/admin/groups?op=listgroup";

    fetch(url_listusr, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.list) setRows(res.list);
        }
        else if (res.errcode === "invalid_session" || res.errcode === "access_denied")
        {
            var backurl = window.location.pathname + window.location.search;
            Utils.clearSession();
            var encodedBackurl = encodeURIComponent(backurl);
            history.push("/signin?back=" + encodedBackurl);
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleClickEdit = (event, item) => {
    history.push("/admin/usergroupedit?group=" + item);
  };

  const handleClickDel = (event, item) => {
    usergroup = item;

    setDelUserGroupDlgOpen(true);
  };

  React.useEffect(() => {
    groupListRefresh();
  }, []);

  // Create New Space dialog
  const [newGroupDlgOpen, setNewGroupDlgOpen] = React.useState(false);

  const handleCloseNewGroupDlg = () => {
    setNewGroupDlgOpen(false);
  };

  const handleGroupNameInputChange = (event) => {
    setGroupName(event.target.value);
  };

  const handleCommentInputChange = (event) => {
    setComment(event.target.value);
  };

  const handleCreateNewGroup = () => {
    if (groupName.length === 0) {
      setNewGroupDlgOpen(false);
      return;
    }

    if (groupName.length > 25) {
      setErrMsg("Group name should be less than 25 characters.");
      return;
    }

    var re = /^[a-zA-Z0-9\_\-]+$/;
    if (!re.test(groupName)) {
      setErrMsg("Invalid characters in group name.");
      return;
    }

    if (comment.length > 512) {
      setErrMsg("Comment should be less than 512 characters.");
      return;
    }

    var url =
      "/admin/groups?op=creategroup&name=" + groupName + "&comment=" + comment;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // refresh display
          groupListRefresh();
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));

    // close dlg
    setNewGroupDlgOpen(false);
  };

  const handleAddButtonClicked = (event) => {
    setNewGroupDlgOpen(true);
  };

  const handleDeleteUserGroup = () => {
    handleCloseDelUserGroupDlg(false);

    var url_deluser = "/admin/usergroups?op=delusergroup&name=" + usergroup;

    fetch(url_deluser, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // refresh display
          groupListRefresh();
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  return (
    <>
      <Paper>
        <TableContainer>
          <Table>
            <TableHead>
              <TableRow>
                {columns.map((column) => (
                  <TableCell
                    key={column.id}
                    align={column.align}
                    style={{ minWidth: column.minWidth }}
                  >
                    <Typography component="div">
                      <Box fontWeight="600" display="inline">
                        {column.label}
                      </Box>
                    </Typography>
                  </TableCell>
                ))}
              </TableRow>
            </TableHead>
            <TableBody>
              {rows.map((row) => {
                return (
                  <TableRow
                    hover
                    role="checkbox"
                    tabIndex={-1}
                    key={row.groupname}
                  >
                    {columns.map((column) => {
                      if (column.id !== "operate") {
                        const value = row[column.id];
                        return (
                          <TableCell key={column.id} align={column.align}>
                            {column.format && typeof value === "number"
                              ? column.format(value)
                              : value}
                          </TableCell>
                        );
                      } else {
                        return (
                          <TableCell key={column.id} align={column.align}>
                            <IconButton
                              onClick={(event) =>
                                handleClickEdit(event, row["groupname"])
                              }
                            >
                              <EditIcon />
                            </IconButton>
                            <IconButton
                              onClick={(event) =>
                                handleClickDel(event, row["groupname"])
                              }
                            >
                              <DeleteIcon />
                            </IconButton>
                          </TableCell>
                        );
                      }
                    })}
                  </TableRow>
                );
              })}
            </TableBody>
          </Table>
        </TableContainer>
      </Paper>

      <p />
      <div style={{ display: "flex" }}>
        <Button
          variant="contained"
          size="small"
          style={{ marginLeft: "auto" }}
          onClick={handleAddButtonClicked}
        >
          {Lang.str_dlg_btn_create}
        </Button>
      </div>
      <Button />

      <Dialog
        open={newGroupDlgOpen}
        onClose={handleCloseNewGroupDlg}
        aria-labelledby="form-new-group-dialog"
      >
        <DialogTitle id="form-new-group-dialog">
          {Lang.str_admin_group_dlg_creategroup}
        </DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            id="groupname"
            value={groupName}
            label={Lang.str_admin_group_name + "*"}
            onChange={handleGroupNameInputChange}
            fullWidth
          />

          <TextField
            margin="dense"
            id="groupcomment"
            value={comment}
            label={Lang.str_admin_group_comment}
            onChange={handleCommentInputChange}
            fullWidth
          />

          <Typography variant="caption" style={{ color: "#ff0000" }}>
            {errmsg}
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCreateNewGroup} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseNewGroupDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>

      <Dialog
        open={delUserGroupDlgOpen}
        onClose={handleCloseDelUserGroupDlg}
        aria-labelledby="delusergroup-dialog-title"
      >
        <DialogTitle id="delusergroup-dialog-title">
          {Lang.str_admin_group_dlg_delgroup}
        </DialogTitle>
        <DialogContent>
          <DialogContentText>
            {Lang.str_admin_dlg_delgroup_confirm}
          </DialogContentText>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleDeleteUserGroup} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseDelUserGroupDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
}
