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

import Dialog from "@material-ui/core/Dialog";
import DialogActions from "@material-ui/core/DialogActions";
import DialogContent from "@material-ui/core/DialogContent";
import DialogContentText from "@material-ui/core/DialogContentText";
import DialogTitle from "@material-ui/core/DialogTitle";
import Button from "@material-ui/core/Button";

import Typography from "@material-ui/core/Typography";

import Utils from "../Utils";

import Lang from "../../i18n/Lang";

import Box from "@material-ui/core/Box";

const columns = [
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
  {
    id: "space",
    label: Lang.str_admin_userview_userspace,
    minWidth: 100,
    //align: 'right',
  },
  {
    id: "operate",
    label: "",
    minWidth: 50,
    align: "right",
  },
];

/*function createData(account, fullname, email, role, space, operate) {
  return { account, fullname, email, role, space, operate };
}*/

var account = "";

export default function UsersView() {
  let history = useHistory();

  const [rows, setRows] = React.useState([]);

  // Delete Page dialog
  const [delUserDlgOpen, setDelUserDlgOpen] = React.useState(false);

  const handleCloseDelUserDlg = () => {
    setDelUserDlgOpen(false);
  };

  const userListRefresh = () => {
    var url_listusr = "/admin/user?op=listuser";

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

  const handleDeleteUser = () => {
    setDelUserDlgOpen(false);

    var url_deluser = "/admin/user?op=deluser&account=" + account;

    fetch(url_deluser, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // refresh display
          userListRefresh();
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleClickEdit = (event, item) => {
    console.log(item);
    history.push("/admin/useredit?account=" + item);
  };

  const handleClickDel = (event, item) => {
    account = item;

    setDelUserDlgOpen(true);
  };

  React.useEffect(() => {
    userListRefresh();
  }, []);

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
                    key={row.account}
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
                                handleClickEdit(event, row["account"])
                              }
                            >
                              <EditIcon />
                            </IconButton>
                            <IconButton
                              onClick={(event) =>
                                handleClickDel(event, row["account"])
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

      <Dialog
        open={delUserDlgOpen}
        onClose={handleCloseDelUserDlg}
        aria-labelledby="deluser-dialog-title"
      >
        <DialogTitle id="deluser-dialog-title">
          {Lang.str_admin_dlg_deluser}
        </DialogTitle>
        <DialogContent>
          <DialogContentText>
            {Lang.str_admin_dlg_deluser_confirm}
          </DialogContentText>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleDeleteUser} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseDelUserDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
}
