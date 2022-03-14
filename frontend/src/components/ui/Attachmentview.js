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

import Button from "@material-ui/core/Button";

import Backdrop from "@material-ui/core/Backdrop";
import CircularProgress from "@material-ui/core/CircularProgress";
import Snackbar from "@material-ui/core/Snackbar";
import Alert from "@material-ui/core/Alert";

const columns = [
  { id: "file", label: Lang.str_fileview_file, minWidth: 110 },
  { id: "size", label: Lang.str_fileview_size, minWidth: 70 },
  {
    id: "date",
    label: Lang.str_fileview_date,
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

class AttachementView extends React.Component {
  state = {
    rows: [],
    selectedFile: null,
    showProgress: false,
    alertOpen: false,
    errmsg: "",
  };

  constructor() {
    super();
  }

  handleAlertClose = (event, reason) => {
    if (reason === "clickaway") {
      return;
    }

    this.setState({
      alertOpen: false,
    });
  };

  getFileList = () => {
    var url = window.location.pathname + "?op=listfile";

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.list) {
            this.setState({ rows: res.list });
          }
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  componentDidMount() {
    this.getFileList();
  }

  onFileChange = (event) => {
    event.preventDefault();
    //console.dir(event);

    // Update the state
    this.setState({ selectedFile: event.target.files[0] });
  };

  onFileUpload = () => {
    if (!this.state.selectedFile) return;

    if (
      !this.state.selectedFile.name ||
      this.state.selectedFile.name.length === 0
    )
      return;

    //var filename = encodeURIComponent(this.state.selectedFile.name);
    var filename = this.state.selectedFile.name;
    var size = this.state.selectedFile.size;

    var reader = new FileReader();

    var _THIS = this;
    reader.onload = function (event) {
      var _filedata = event.target.result;

      var ibegin = _filedata.indexOf(";base64,");
      if (ibegin === -1) return;

      ibegin = ibegin + ";base64,".length;
      var filedata = _filedata.substring(ibegin);

      var url = window.location.pathname + "?op=upload&filename=" + filename;

      _THIS.setState({ showProgress: true });

      fetch(url, {
        method: "post",
        body: JSON.stringify({ sid: Utils.getSessionId(), size, filedata }),
      })
        .then((response) => response.text())
        .then((data) => {
          var res = JSON.parse(data);

          _THIS.setState({ showProgress: false });

          if (res.status === "ok") {
            _THIS.getFileList();
          } else if (res.status === "fail") {
            if (res.errcode && res.errcode === "file_exceeds_limit") {
              _THIS.setState({
                alertOpen: true,
                errmsg: "File exceeds limit.",
              });
            } else if (res.errcode === "invalid_session") {
              Utils.clearSession();
              _THIS.props.history.push("/signin");
            }
          }
        })
        .catch((error) => console.log("error is", error));
    };

    reader.readAsDataURL(this.state.selectedFile); // increase 33.3% size
  };

  render() {
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
                {this.state.rows.reverse().map((row) => {
                  return (
                    <TableRow
                      hover
                      role="checkbox"
                      tabIndex={-1}
                      key={row.file}
                    >
                      {columns.map((column) => {
                        if (column.id === "operate") {
                          return (
                            <TableCell
                              key={column.id + "_op"}
                              align={column.align}
                            >
                              {""}
                            </TableCell>
                          );
                        } else if (column.id === "file") {
                          const value = row[column.id];
                          return (
                            <TableCell key={column.id} align={column.align}>
                              <a
                                href={
                                  window.location.pathname +
                                  "/upload/" +
                                  value
                                }
                                download={value}
                              >
                                {value}
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

        <p />

        {/*
        <div>
          <input type="file" onChange={this.onFileChange} />

          <Button
            variant="contained"
            size="small"
            style={{ marginLeft: "auto" }}
            onClick={this.onFileUpload}
          >
            {Lang.str_fileview_upload}
          </Button>
        </div>
        */}
        <p />

        <p />
        <Backdrop
          sx={{ color: "#fff", zIndex: (theme) => theme.zIndex.drawer + 1 }}
          open={this.state.showProgress}
        >
          <CircularProgress color="inherit" />
        </Backdrop>

        <Snackbar
          open={this.state.alertOpen}
          anchorOrigin={{ vertical: "top", horizontal: "center" }}
          onClose={this.handleAlertClose}
        >
          <Alert onClose={this.handleAlertClose} severity="error">
            {" "}
            {this.state.errmsg}{" "}
          </Alert>
        </Snackbar>
      </>
    );
  }
}

export default withRouter(AttachementView);
