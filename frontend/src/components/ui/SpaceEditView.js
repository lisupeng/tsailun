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
import Typography from "@material-ui/core/Typography";
import Checkbox from "@material-ui/core/Checkbox";
import { OutlinedInput } from "@material-ui/core";

import Utils from "../Utils";
import Lang from "../../i18n/Lang";

const grayedOutInputStyle = {
  backgroundColor: "lightgrey",
};

const permissionTableColumns = [
  { id: "item", label: Lang.str_space_edit_grant_to, minWidth: 120 },
  { id: "itemtype", label: Lang.str_space_edit_type, minWidth: 120 },
  { id: "read", label: Lang.str_space_edit_read_permission, minWidth: 30 },
  { id: "write", label: Lang.str_space_edit_write_permission, minWidth: 30 },
];

function addUserListToPermisionTable(tablerows, userlist) {
  for (var i in userlist) {
    var entry = userlist[i];

    if (entry && entry.account) {
      var item = entry.account;
      var itemtype = "user";
      var read = false;
      var write = false;

      tablerows.push({ item, itemtype, read, write });
    }
  }
}

function addGroupListToPermisionTable(tablerows, grouplist) {
  console.log(grouplist);

  for (var i in grouplist) {
    var entry = grouplist[i];

    if (entry && entry.groupname) {
      var item = entry.groupname;
      var itemtype = "group";
      var read = false;
      var write = false;

      tablerows.push({ item, itemtype, read, write });
    }
  }
}

function updatePermissionFlags(permissionTableRows, rlist, wlist) {
  var RlookupSet = new Set();
  var WlookupSet = new Set();

  for (var i in rlist) {
    var e = rlist[i];
    RlookupSet.add(e.item + "$" + e.itemtype);
  }

  for (var i in wlist) {
    var e = wlist[i];
    WlookupSet.add(e.item + "$" + e.itemtype);
  }

  for (var i in permissionTableRows) {
    var e = permissionTableRows[i];

    var str = e.item + "$" + e.itemtype;

    if (RlookupSet.has(str)) e.read = true;
    else e.read = false;

    if (WlookupSet.has(str)) e.write = true;
    else e.write = false;
  }
}

export default function SpaceEditView() {
  let history = useHistory();

  const [spacename, setSpacename] = React.useState("");
  const [path, setPath] = React.useState("");

  const [ptableRows, setPtableRows] = React.useState([]);

  const handleClickCheckbox = (event, item, itemtype, checkbox) => {
    console.log("click() " + item + " " + itemtype + " " + checkbox);

    for (var i in ptableRows) {
      var e = ptableRows[i];

      if (e.item === item && e.itemtype === itemtype) {
        if (checkbox === "read") e.read = !e.read;
        else e.write = !e.write;

        var newlist = JSON.parse(JSON.stringify(ptableRows));

        setPtableRows(newlist);

        return;
      }
    }
  };

  React.useEffect(() => {
    var queryobj = Utils.parse(window.location.href);
    if (!queryobj.space) return;

    var url = "/admin/spaces?op=getspaceinfo&space=" + queryobj.space;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.spaceinfo.spacename) setSpacename(res.spaceinfo.spacename);

          if (res.spaceinfo.path) setPath(res.spaceinfo.path);

          var url_listusr = "/admin/user?op=listuser";

          fetch(url_listusr, {
            method: "post",
            body: JSON.stringify({ sid: Utils.getSessionId() }),
          })
            .then((response) => response.text())
            .then((data) => {
              var res2 = JSON.parse(data);
              if (res2.status === "ok") {
                var url_listgroup = "/admin/groups?op=listgroup";

                fetch(url_listgroup, {
                  method: "post",
                  body: JSON.stringify({ sid: Utils.getSessionId() }),
                })
                  .then((response) => response.text())
                  .then((data) => {
                    var res3 = JSON.parse(data);
                    if (res3.status === "ok") {
                      var pRows = [];

                      if (res2.list) {
                        addUserListToPermisionTable(pRows, res2.list);
                      }

                      if (res3.list) {
                        addGroupListToPermisionTable(pRows, res3.list);
                      }

                      updatePermissionFlags(
                        pRows,
                        res.spaceinfo.rlist,
                        res.spaceinfo.wlist
                      );

                      setPtableRows(pRows);
                    }
                  })
                  .catch((error) => console.log("error is", error));
              }
            })
            .catch((error) => console.log("error is", error));
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  }, []);

  const handleClickUpdateSpace = (event) => {
    var rlist = [];
    var wlist = [];

    for (var i in ptableRows) {
      var e = ptableRows[i];

      if (e.read === true) rlist.push({ item: e.item, itemtype: e.itemtype });

      if (e.write === true) wlist.push({ item: e.item, itemtype: e.itemtype });
    }

    var url_updategroup = "/admin/spaces?op=updatespace";

    fetch(url_updategroup, {
      method: "post",
      body: JSON.stringify({
        sid: Utils.getSessionId(),
        spacename: spacename,
        rlist,
        wlist,
      }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          history.push("/admin/spaces");
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
            {Lang.str_admin_space_name}
          </Typography>

          <OutlinedInput
            value={spacename}
            placeholder=""
            size="small"
            disabled
            style={grayedOutInputStyle}
          ></OutlinedInput>

          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_space_path}
          </Typography>

          <OutlinedInput
            value={path}
            placeholder=""
            size="small"
            disabled
            style={grayedOutInputStyle}
          ></OutlinedInput>

          <p />

          <TableContainer sx={{ maxHeight: 400 }}>
            <Table>
              <TableHead>
                <TableRow>
                  {permissionTableColumns.map((column) => (
                    <TableCell
                      key={column.id}
                      align={column.align}
                      style={{ minWidth: column.minWidth }}
                    >
                      <Typography variant="body2" style={{ color: "#1976d2" }}>
                        {column.label}
                      </Typography>
                    </TableCell>
                  ))}
                </TableRow>
              </TableHead>
              <TableBody>
                {ptableRows.map((row) => {
                  return (
                    <TableRow
                      hover
                      role="checkbox"
                      tabIndex={-1}
                      key={row.item + row.itemtype}
                    >
                      {permissionTableColumns.map((column) => {
                        if (column.id === "read" || column.id === "write") {
                          var isChecked = row[column.id];
                          return (
                            <TableCell key={column.id} align={column.align}>
                              <Checkbox
                                color="primary"
                                checked={isChecked}
                                size="small"
                                onClick={(event) =>
                                  handleClickCheckbox(
                                    event,
                                    row.item,
                                    row.itemtype,
                                    column.id
                                  )
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
          onClick={(event) => handleClickUpdateSpace(event)}
        >
          {Lang.str_dlg_btn_save}
        </Button>
      </>
    </React.Fragment>
  );
}
