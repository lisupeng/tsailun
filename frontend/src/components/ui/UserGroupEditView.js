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

//import IconButton from '@material-ui/core/IconButton';
//import EditIcon from '@material-ui/icons/Edit';
//import DeleteIcon from '@material-ui/icons/Delete';

import Button from "@material-ui/core/Button";

//
//import Grid from '@material-ui/core/Grid';
import Typography from "@material-ui/core/Typography";
//import TextField from '@material-ui/core/TextField';
//import FormControlLabel from '@material-ui/core/FormControlLabel';
import Checkbox from "@material-ui/core/Checkbox";
import { OutlinedInput } from "@material-ui/core";

//import MenuItem from '@material-ui/core/MenuItem';
//import Select from '@material-ui/core/Select';

//import AddIcon from '@material-ui/icons/Add';

import Utils from "../Utils";

import Lang from "../../i18n/Lang";

import Box from "@material-ui/core/Box";

const grayedOutInputStyle = {
  backgroundColor: "lightgrey",
};

const selUserTableColumns = [
  { id: "select", label: Lang.str_admin_groupedit_choose_member, minWidth: 30 },
  { id: "account", label: Lang.str_admin_userview_account, minWidth: 70 },
  { id: "fullname", label: Lang.str_admin_userview_name, minWidth: 70 },
  {
    id: "email",
    label: Lang.str_admin_userview_email,
    minWidth: 100,
    //align: 'right',
  },
  {
    id: "role",
    label: Lang.str_admin_userview_role,
    minWidth: 70,
    //align: 'right',
  },
];

function addUserListToSelUserTable(tablerows, userlist) {
  for (var i in userlist) {
    var entry = userlist[i];

    if (entry && entry.account) {
      var select = false;
      var account = entry.account;
      var fullname = entry.fullname;
      var email = entry.email;
      var role = entry.role;

      tablerows.push({ select, account, fullname, email, role });
    }
  }
}

function updateSelUserTableCheckbox(selUserTableRows, userlist) {
  var UserlookupSet = new Set();

  for (var i in userlist) {
    var account = userlist[i];
    UserlookupSet.add(account);
  }

  for (var i in selUserTableRows) {
    var e = selUserTableRows[i];

    var account = e.account;

    if (UserlookupSet.has(account)) e.select = true;
    else e.select = false;
  }
}

export default function UserGroupEditView() {
  let history = useHistory();

  const [groupname, setGroupname] = React.useState("");
  const [comment, setComment] = React.useState("");

  const [selUserTableRows, setSelUserTableRows] = React.useState([]);

  React.useEffect(() => {
    var queryobj = Utils.parse(window.location.href);
    if (!queryobj.group) return;

    var url = "/admin/groups?op=getgroupinfo&group=" + queryobj.group;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.groupinfo.groupname) setGroupname(res.groupinfo.groupname);

          if (res.groupinfo.comment) setComment(res.groupinfo.comment);

          var url_listusr = "/admin/user?op=listuser";

          fetch(url_listusr, {
            method: "post",
            body: JSON.stringify({ sid: Utils.getSessionId() }),
          })
            .then((response) => response.text())
            .then((data) => {
              var res2 = JSON.parse(data);
              if (res2.status === "ok") {
                if (res2.list) {
                  var selRows = [];
                  addUserListToSelUserTable(selRows, res2.list);

                  if (res.groupinfo && res.groupinfo.userlist)
                    updateSelUserTableCheckbox(selRows, res.groupinfo.userlist);

                  setSelUserTableRows(selRows);
                }
              }
            })
            .catch((error) => console.log("error is", error));
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  }, []);

  const handleClickCheckbox = (event, account) => {
    for (var i in selUserTableRows) {
      var e = selUserTableRows[i];

      if (e.account === account) {
        e.select = !e.select;

        var newlist = JSON.parse(JSON.stringify(selUserTableRows));

        setSelUserTableRows(newlist);

        return;
      }
    }
  };

  const handleCommentChange = (event) => {
    setComment(event.target.value);
  };

  const handleClickUpdateGroup = (event) => {
    // groupname

    var userlist = [];

    for (var i in selUserTableRows) {
      var entry = selUserTableRows[i];

      if (entry && entry.select === true) {
        userlist.push(entry.account);
      }
    }

    var url_updategroup = "/admin/group?op=updategroup";

    fetch(url_updategroup, {
      method: "post",
      body: JSON.stringify({
        sid: Utils.getSessionId(),
        group: groupname,
        comment,
        userlist,
      }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          history.push("/admin/usergroups");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  return (
    <React.Fragment>
      <>
        <Paper
          sx={{
            p: 2,
            display: "flex",
            flexDirection: "column",
          }}
        >
          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_group_name}
          </Typography>

          <OutlinedInput
            value={groupname}
            placeholder=""
            size="small"
            disabled
            style={grayedOutInputStyle}
          ></OutlinedInput>

          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_group_comment}
          </Typography>

          <OutlinedInput
            value={comment}
            onChange={handleCommentChange}
            placeholder=""
            size="small"
          ></OutlinedInput>

          <p />

          <TableContainer sx={{ maxHeight: 400 }}>
            <Table>
              <TableHead>
                <TableRow>
                  {selUserTableColumns.map((column) => (
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
                {selUserTableRows.map((row) => {
                  return (
                    <TableRow
                      hover
                      role="checkbox"
                      tabIndex={-1}
                      key={row.account}
                    >
                      {selUserTableColumns.map((column) => {
                        if (column.id === "select") {
                          var isChecked = row[column.id];
                          return (
                            <TableCell key={column.id} align={column.align}>
                              <Checkbox
                                color="primary"
                                checked={isChecked}
                                size="small"
                                onClick={(event) =>
                                  handleClickCheckbox(event, row.account)
                                }
                              />
                            </TableCell>
                          );
                        } else {
                          const value = row[column.id];
                          return (
                            <TableCell key={column.id} align={column.align}>
                              {column.format && typeof value === "number"
                                ? column.format(value)
                                : value}
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

          <p />
        </Paper>

        <p />
        <Button
          variant="contained"
          size="small"
          onClick={(event) => handleClickUpdateGroup(event)}
        >
          {Lang.str_dlg_btn_save}
        </Button>
      </>
    </React.Fragment>
  );
}
