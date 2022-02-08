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
import Editorwrap from "./Editorwrap";
import Pageviewer from "./Pageviewer";
import Historyviewer from "./Historyviewer";
import AttachementView from "./Attachmentview";
import EmptySpacePrompt from "./Emptyspaceprompt";
import Utils from "../Utils";
import Snackbar from "@material-ui/core/Snackbar";
import Alert from "@material-ui/core/Alert";

var cache_pageListUrl = "";
var cache_pageListRes;
var cache_pageDataUrl = "";
var cache_pageData;

var uiType;

class Contentarea extends React.Component {
  state = {
    alertOpen: false,
    errmsg: "",
  };

  constructor(props) {
    super();
  }

  componentDidMount() {}

  handleAlertClose = (event, reason) => {
    if (reason === "clickaway") {
      return;
    }

    this.setState({
      alertOpen: false,
    });
  };

  render() {
    var shouldGetPageList = false;

    const handleListPageResult = (res, usingCache) => {
      if (res.status === "ok") {
        if (res.list && res.list.length === 0) {
          //space 'empty';
          uiType = "emptySpacePrompt";
          if (usingCache === false) {
            this.forceUpdate();
          }
        } else {
          // space 'non-empty';
          // redirect to first page of space
          if (
            res.list &&
            res.list.length > 0 &&
            res.list[0] &&
            res.list[0].pagedir
          ) {
            var url_redirect =
              Utils.getSpacePathFromUrl(window.location.pathname) +
              "/pages/" +
              res.list[0].pagedir +
              "?op=view";
            this.props.history.push(url_redirect);
          }
        }
      } else {
        if (res.errcode === "invalid_session") {
          // redirect to signin
          Utils.clearSession();
          this.props.history.push("/signin");
        } else if (res.errcode === "access_denied") {
          if (usingCache === false) {
            this.setState({
              alertOpen: true,
              errmsg: "Access denied.",
            });
          }
        } else {
          console.log("Unexpected error");
        }
      }
    };

    var queryobj = Utils.parse(window.location.href);
    if (queryobj.op) {
      // has 'op='
      if (
        queryobj.op === "edit" ||
        queryobj.op === "history" ||
        queryobj.op === "attachment"
      )
        uiType = queryobj.op;
      else if (queryobj.op === "view") uiType = "pageviewer";
    } else if (!queryobj.op) {
      // no 'op='
      // window.location.pathname typical value: '/spaces/testspace/pages/'
      if (Utils.PathHasSpaceInfo(window.location.pathname)) {
        uiType = "pageviewer";

        if (!Utils.PathHasPageInfo(window.location.pathname)) {
          shouldGetPageList = true;
        }
      }
    }

    // get page list and redirect to first page
    if (shouldGetPageList === true) {
      var url_listpage =
        Utils.getSpacePathFromUrl(window.location.pathname) +
        "/pages?op=listpage";
      if (cache_pageListUrl !== url_listpage) {
        cache_pageListUrl = url_listpage;
        cache_pageListRes = null;

        // The reason put session id to post:
        //    User may save url for future use. It's better not put it to url
        fetch(url_listpage, {
          method: "post",
          body: JSON.stringify({ sid: Utils.getSessionId() }),
        })
          .then((response) => response.text())
          .then((data) => {
            var res = JSON.parse(data);

            cache_pageListRes = res;

            handleListPageResult(res, false);
          })
          .catch((error) => console.log("error is", error));
      } // === cache_pageListUrl
      else {
        if (cache_pageListRes !== null) {
          handleListPageResult(cache_pageListRes, true);
        }
      }
    }

    return (
      <>
        <Snackbar
          open={this.setState.alertOpen}
          anchorOrigin={{ vertical: "top", horizontal: "center" }}
          onClose={this.handleAlertClose}
        >
          <Alert onClose={this.handleAlertClose} severity="error">
            {" "}
            {this.errmsg}{" "}
          </Alert>
        </Snackbar>

        {uiType === "edit" && <Editorwrap />}
        {uiType === "history" && <Historyviewer />}
        {uiType === "attachment" && <AttachementView />}
        {uiType === "pageviewer" && <Pageviewer />}
        {uiType === "emptySpacePrompt" && <EmptySpacePrompt />}
      </>
    );
  }
}

export default withRouter(Contentarea);
