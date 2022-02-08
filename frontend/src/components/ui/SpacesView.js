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

import IconButton from "@material-ui/core/IconButton";
import EditIcon from "@material-ui/icons/Edit";
import DeleteIcon from "@material-ui/icons/Delete";

import Button from "@material-ui/core/Button";

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

const columns = [
  { id: "spacename", label: Lang.str_admin_space_name, minWidth: 120 },
  { id: "path", label: Lang.str_admin_space_path, minWidth: 280 },
  /*
  {
    id: 'rcount',
    label: 'Reader List',
    minWidth: 100,
  },
  {
    id: 'wcount',
    label: 'Writer List',
    minWidth: 100,
  },
  */
  {
    id: "operate",
    label: "",
    minWidth: 120,
    align: "right",
  },
];

/*function createData(spacename, path, rcount, wcount, operate) {
  return { spacename, path, rcount, wcount, operate };
}*/

/*
const rows = [
  createData('space_default', '/spaces/default', '107 item(s)', '100 item(s)', 'Del|Edit'),

];
*/

var spacenameDel = "";

export default function SpacesView() {
  let history = useHistory();

  const [rows, setRows] = React.useState([]);

  const [errmsg, setErrMsg] = React.useState("");

  const [spaceName, setSpaceName] = React.useState("");
  const [spacePath, setSpacePath] = React.useState("");

  const [delSpaceDlgOpen, setDelSpaceDlgOpen] = React.useState(false);
  const handleCloseDelSpaceDlg = () => {
    setDelSpaceDlgOpen(false);
  };

  const handleClickEdit = (event, item) => {
    history.push("/admin/spaceedit?space=" + item);
  };

  const handleClickDel = (event, item) => {
    spacenameDel = item;

    setDelSpaceDlgOpen(true);
  };

  const spaceListRefresh = () => {
    var url_listusr = "/admin/spaces?op=listspace";

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
      })
      .catch((error) => console.log("error is", error));
  };

  React.useEffect(() => {
    spaceListRefresh();
  }, []);

  const handleAddButtonClicked = (event) => {
    setErrMsg("");
    setNewSpaceDlgOpen(true);
  };

  // Create New Space dialog
  const [newSpaceDlgOpen, setNewSpaceDlgOpen] = React.useState(false);

  const handleCloseNewSpaceDlg = () => {
    setNewSpaceDlgOpen(false);
  };

  const handleSpaceNameInputChange = (event) => {
    setSpaceName(event.target.value);
  };

  const handleSpacePathInputChange = (event) => {
    setSpacePath(event.target.value);
  };

  const handleCreateNewSpace = () => {
    if (spaceName.length === 0 || spacePath.length === 0) {
      return;
    }

    // check spaceName
    if (spaceName.length > 25) {
      setErrMsg("Space name should be less than 25 characters.");
      return;
    }

    var re = /^[a-zA-Z0-9\_\-]+$/;
    if (!re.test(spaceName)) {
      setErrMsg("Invalid characters in space name.");
      return;
    }

    // check spacePath
    var rePath = /[\:\*\?\"\<\>\.\|]+/;
    if (rePath.test(spacePath)) {
      setErrMsg("Invalid characters in path.");
      return;
    }

    var __path = "";

    if (spacePath.startsWith("/spaces/")) {
      __path = spacePath;
    } else if (spacePath.startsWith("/")) {
      __path = "/spaces" + spacePath;
    } else {
      __path = "/spaces/" + spacePath;
    }

    var url =
      "/admin/spaces?op=createspace&name=" + spaceName + "&path=" + __path;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // refresh display
          spaceListRefresh();
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));

    // close dlg
    setNewSpaceDlgOpen(false);
  };

  const handleDeleteSpace = () => {
    setDelSpaceDlgOpen(false);

    var url_deluser = "/admin/spaces?op=delspace&name=" + spacenameDel;

    fetch(url_deluser)
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          spaceListRefresh();
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
                    {/*<Typography variant="body2"  style={{ color: "#1976d2" }}>*/}

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
                    key={row.spacename}
                  >
                    {columns.map((column) => {
                      if (column.id !== "operate") {
                        const value = row[column.id];
                        return (
                          <TableCell key={column.id} align={column.align}>
                            {value ? value : "-"}
                          </TableCell>
                        );
                      } else {
                        return (
                          <TableCell key={column.id} align={column.align}>
                            <IconButton
                              onClick={(event) =>
                                handleClickEdit(event, row["spacename"])
                              }
                            >
                              <EditIcon />
                            </IconButton>
                            <IconButton
                              onClick={(event) =>
                                handleClickDel(event, row["spacename"])
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
        open={newSpaceDlgOpen}
        onClose={handleCloseNewSpaceDlg}
        aria-labelledby="form-new-space-dialog"
      >
        <DialogTitle id="form-new-space-dialog">
          {Lang.str_admin_dlg_createspace}
        </DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            id="spacename"
            value={spaceName}
            label={Lang.str_admin_space_name + "*"}
            onChange={handleSpaceNameInputChange}
            fullWidth
          />
          <TextField
            margin="dense"
            id="spacepath"
            value={spacePath}
            label={Lang.str_admin_space_path + "*"}
            onChange={handleSpacePathInputChange}
            fullWidth
          />

          <Typography variant="caption" style={{ color: "#ff0000" }}>
            {errmsg}
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCreateNewSpace} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseNewSpaceDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>

      <Dialog
        open={delSpaceDlgOpen}
        onClose={handleCloseDelSpaceDlg}
        aria-labelledby="delspace-dialog-title"
      >
        <DialogTitle id="delspace-dialog-title">
          {Lang.str_dlg_delspace}
        </DialogTitle>
        <DialogContent>
          <DialogContentText>{Lang.str_delspace_confirm}</DialogContentText>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleDeleteSpace} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseDelSpaceDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
}
