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
//import PropTypes from 'prop-types';
import { withRouter } from "react-router-dom";
import Utils from "../Utils";
import Typography from "@material-ui/core/Typography";

import IconButton from "@material-ui/core/IconButton";
import EditIcon from "@material-ui/icons/Edit";

//import { useHistory } from 'react-router-dom';
//import {withRouter} from 'react-router-dom';
//import { createBrowserHistory, createHashHistory } from 'history';

import Snackbar from "@material-ui/core/Snackbar";
import Alert from "@material-ui/core/Alert";

import Globaldata from "../Globaldata";

import Lang from "../../i18n/Lang";
import Fab from "@material-ui/core/Fab";
import Backdrop from "@material-ui/core/Backdrop";
import CircularProgress from "@material-ui/core/CircularProgress";

//import Paper from '@material-ui/core/Paper';

var pagetitle = "";

var author = "";
var authorFullname = "";

var createDateUTC = "";
var lastModifiedBy = "";

var lastByFullname = "";

var lastModifiedDate = "";

var thisptr;

var _space_last_check = "";
var _space_empty_check_status = ""; // 'inprogress', 'non-empty', 'empty'

const editBtnStyle = {
  margin: 0,
  top: "auto",
  right: 30,
  bottom: 40,
  left: "auto",
  position: "fixed",
};

class Pageviewer extends React.Component {
  state = {
    alertOpen: false,
    errmsg: "",
    showProgress: false,
  };

  constructor(props) {
    super();
    this.msg = "";
    this.path_rendering = "";
  }

  componentDidMount() {
  }

  clearPageContent() {
    pagetitle = "";
    author = "";
    lastModifiedBy = "";
    lastModifiedDate = "";
    this.msg = "";
  }

  handleAlertClose = (event, reason) => {
    if (reason === "clickaway") {
      return;
    }

    this.setState({
      alertOpen: false,
    });
  };

  onEditButtonClicked = () => {
    var page_full_path = window.location.pathname;
    var url_editpage = page_full_path + "?op=edit";

    var url_checkpermission =
      page_full_path + "?op=checkpermission&access=writespace";

    fetch(url_checkpermission, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          thisptr.props.history.push(url_editpage);
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            this.props.history.push("/signin");
          } else if (res.errcode === "access_denied") {
            this.setState({
              alertOpen: true,
              errmsg: "Access denied.",
            });
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };

  renderEditButton() {
    //var queryobj = Utils.parse(window.location.href);
    // don't show EDIT button if specified version
    //if (!(queryobj.ver)) {

    if (thisptr && thisptr.msg && thisptr.msg.length !== 0) {
      return (
        <div style={{ display: "flex" }}>
          <IconButton
            style={{ marginLeft: "auto" }}
            size="small"
            onClick={thisptr.onEditButtonClicked}
          >
            <EditIcon />
            {Lang.str_pageview_edit}
          </IconButton>
          {/*<a href={window.location.pathname+'?op=edit'} style={{ marginLeft: "auto" }}>Edit</a>*/}
        </div>
      );
    } else {
      return <></>;
    }
    //}
  }

  renderPageHead() {
    var localDate = Utils.UtcDateToLocalDate2(lastModifiedDate);
    var createDate = Utils.UtcDateToLocalDate2(createDateUTC);

    var _by;
    if (authorFullname.length > 0) _by = authorFullname;
    else _by = author;

    var _last;
    if (lastByFullname.length > 0) _last = lastByFullname;
    else _last = lastModifiedBy;
    
    if (pagetitle.length > 0)
        document.title = pagetitle + ' - tsailun';
    else
        document.title = "Tsailun";

    if (thisptr && thisptr.msg && thisptr.msg.length !== 0) {
      return (
        <>
          <Typography variant="h4">{pagetitle}</Typography>

          {_by && _by.length > 0 && _last && _last.length > 0 && (
            <Typography variant="caption" style={{ color: "#7b7b7b" }}>
              {Utils.sprint(Lang.str_pageview_author_modify_date, {
                author: _by,
                createDate,
                lastModifiedBy: _last,
                localDate,
              })}
            </Typography>
          )}

          <div dangerouslySetInnerHTML={{ __html: "<br>" }}></div>

          <p />
        </>
      );
    } else {
      return <></>;
    }
  }

  render() {
    // fetch page data if url changed
    if (
      Globaldata.readingPage !== true &&
      (this.path_rendering !== window.location.pathname ||
        Globaldata.invalidatePageCache === true)
    ) {
      this.clearPageContent();
      Globaldata.invalidatePageCache = false;

      var page_full_path = window.location.pathname;
      var url_readpage = page_full_path + "?op=read";
      var queryobj = Utils.parse(window.location.href);

      if (queryobj.ver) {
        url_readpage = url_readpage + "&ver=" + queryobj.ver;
      }

      var backurl = window.location.pathname + window.location.search;

      Globaldata.readingPage = true;

      setTimeout(() => {
        if (Globaldata.readingPage === true) {
          this.setState({ showProgress: true });
        }
      }, 2000);

      // get page data
      fetch(url_readpage, {
        method: "post",
        body: JSON.stringify({ sid: Utils.getSessionId() }),
      })
        .then((response) => response.text())
        .then((data) => {
          Globaldata.readingPage = false;
          this.setState({ showProgress: false });
          this.path_rendering = page_full_path;

          var res = JSON.parse(data);
          if (res.status === "ok") {
            if (res.body) {
              //console.log("body="+res.body);
              this.msg = res.body;

              if (res.title) {
                pagetitle = res.title;
                Globaldata.titleOfCurDisplayedPage = res.title;
              } else {
                pagetitle = "";
                Globaldata.titleOfCurDisplayedPage = "";
              }

              if (res.author) {
                author = res.author;
              } else author = "";

              if (res.author_fullname) authorFullname = res.author_fullname;
              else authorFullname = "";

              if (res.created_date) createDateUTC = res.created_date;
              else createDateUTC = "";

              if (res.last_by) {
                lastModifiedBy = res.last_by;
              } else lastModifiedBy = "";

              if (res.lasyby_fullname) lastByFullname = res.lasyby_fullname;
              else lastByFullname = "";

              if (res.last_modified_date) {
                lastModifiedDate = res.last_modified_date;
              } else lastModifiedDate = "";

              // Force rendering
              this.forceUpdate();
            }
          } else {
            if (res.errcode === "invalid_session") {
              // redirect to signin
              Utils.clearSession();
              var encodedBackurl = encodeURIComponent(backurl);
              //console.dir(encodedBackurl);
              this.props.history.push("/signin?back=" + encodedBackurl);
            } else if (res.errcode === "access_denied") {
              this.setState({
                alertOpen: true,
                errmsg: "Access denied.",
              });
            } else {
              console.log("Unexpected error");
            }
          }
        })
        .catch((error) => {
          console.log("error is", error);
          Globaldata.readingPage = false;
          this.setState({ showProgress: false });
        });
    }

    thisptr = this;

    // TODO use cipher algorithm to make sure no data corrupt
    return (
      <>
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

        {this.state.showProgress && (
          <Backdrop
            sx={{ color: "#fff", zIndex: (theme) => theme.zIndex.drawer + 1 }}
            open={true}
          >
            <CircularProgress color="inherit" />
          </Backdrop>
        )}

        {/*TODO: make progress indicator looks better <CircularProgress color="primary" sx={{position: 'fixed', top: window.innerWidth/2, left: window.innerHeight/2, zIndex: 1500}}/>*/}

        {thisptr && thisptr.msg && thisptr.msg.length !== 0 && (
          <Fab
            color="primary"
            size="small"
            aria-label="edit"
            style={editBtnStyle}
            onClick={thisptr.onEditButtonClicked}
          >
            <EditIcon />
          </Fab>
        )}

        {/*<this.renderEditButton />*/}

        <Typography variant="body2" style={{ whiteSpace: "pre-line" }}>
          {"\n"}
        </Typography>

        <this.renderPageHead />

        <div
          id="pgviewer"
          dangerouslySetInnerHTML={{
            __html: "<style>p{ margin:2px 0}</style>" + '<style type="text/css"> @import "/tinymce/js/tinymce/skins/ui/oxide/content.min.css" </style>' + this.msg,
          }}
        ></div>
      </>
    );
  }
}

export default withRouter(Pageviewer);
