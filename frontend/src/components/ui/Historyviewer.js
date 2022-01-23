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
import { withRouter } from "react-router-dom";
import Utils from "../Utils";
import Globaldata from "../Globaldata";

import Paper from "@material-ui/core/Paper";
import Table from "@material-ui/core/Table";
import TableBody from "@material-ui/core/TableBody";
import TableCell from "@material-ui/core/TableCell";
import TableContainer from "@material-ui/core/TableContainer";
import TableHead from "@material-ui/core/TableHead";
//import TablePagination from '@material-ui/core/TablePagination';
import TableRow from "@material-ui/core/TableRow";

import Lang from "../../i18n/Lang";

import Box from "@material-ui/core/Box";
import Typography from "@material-ui/core/Typography";

const columns = [
  { id: "version", label: Lang.str_historyview_version, minWidth: 50 },
  { id: "author", label: Lang.str_historyview_author, minWidth: 70 },
  {
    id: "date",
    label: Lang.str_historyview_date,
    minWidth: 100,
    //align: 'right',
  },
  {
    id: "preview",
    label: "",
    minWidth: 50,
  },
  {
    id: "operate",
    label: "",
    minWidth: 50,
    align: "right",
  },
];

/*function createData(version, author, date, operate) {
  return { version, author, date, operate };
}*/

class Historyviewer extends React.Component {
  state = {
    rows: [],
  };

  constructor() {
    super();
  }

  componentDidMount() {
    var page_full_path = window.location.pathname;
    var url_listver = page_full_path + "?op=listversion";

    fetch(url_listver)
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          Globaldata.versionList = res.list;
          this.setState({
            rows: res.list,
          });
        }
      })
      .catch((error) => console.log("error is", error));
  }

  onRestoreClicked(ver, e) {
    e.preventDefault();

    var url_restore = window.location.pathname + "?op=restore&ver=" + ver;

    // call restore api
    fetch(url_restore, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // redirect to op=view
          var url_view = window.location.pathname + "?op=view";
          this.props.history.push(url_view);
        }
      })
      .catch((error) => console.log("error is", error));
  }

  render() {
    return (
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
              {this.state.rows.reverse().map((row) => {
                return (
                  <TableRow
                    hover
                    role="checkbox"
                    tabIndex={-1}
                    key={row.version}
                  >
                    {columns.map((column) => {
                      if (column.id === "version") {
                        return (
                          <TableCell key={column.id} align={column.align}>
                            <a
                              href={
                                window.location.pathname +
                                "?op=view&ver=" +
                                row["version"]
                              }
                            >
                              {row["version"]}
                            </a>
                          </TableCell>
                        );
                      } else if (column.id === "preview") {
                        return (
                          <TableCell key={column.id} align={column.align}>
                            <a
                              href={
                                window.location.pathname +
                                "?op=view&ver=" +
                                row["version"]
                              }
                            >
                              {Lang.str_historyview_view}
                            </a>
                          </TableCell>
                        );
                      } else if (column.id === "operate") {
                        return (
                          <TableCell key={column.id} align={column.align}>
                            <a
                              href={
                                window.location.pathname +
                                "?op=restore&ver=" +
                                row["version"]
                              }
                              onClick={this.onRestoreClicked.bind(
                                this,
                                row["version"]
                              )}
                            >
                              {Lang.str_historyview_restore}
                            </a>
                          </TableCell>
                        );
                      } else if (column.id === "date") {
                        const value = row[column.id];
                        return (
                          <TableCell key={column.id} align={column.align}>
                            {Utils.UtcDateToLocalDate2(value)}
                          </TableCell>
                        );
                      } else {
                        const value = row[column.id];
                        return (
                          <TableCell key={column.id} align={column.align}>
                            {value}
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
    );
  }
}

export default withRouter(Historyviewer);
