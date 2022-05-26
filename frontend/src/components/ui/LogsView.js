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
import Typography from "@material-ui/core/Typography";
import Utils from "../Utils";
import { useHistory } from "react-router-dom";
//import Globaldata from '../Globaldata';

export default function LogsView() {
  let history = useHistory();
  const [logmsg, setLogMsg] = React.useState("");

  const fetchLog = () => {
    var url_getlog = "/admin/logs?op=getlog";

    fetch(url_getlog, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.log) setLogMsg(res.log);
        }
        else if (res.errcode === "invalid_session") {
            var backurl = window.location.pathname + window.location.search;
            Utils.clearSession();
            var encodedBackurl = encodeURIComponent(backurl);
            history.push("/signin?back=" + encodedBackurl);
        }
      })
      .catch((error) => console.log("error is", error));
  };

  React.useEffect(() => {
    fetchLog();
  }, []);

  return (
    <>
      {/*<Paper sx={{ p: 2, display: 'flex', flexDirection: 'column', height: `calc(100vh - ${Globaldata.appBarHeight}px)`}}>*/}

      <Typography variant="body2" style={{ whiteSpace: "pre-line" }}>
        {logmsg}
      </Typography>

      {/*</Paper>*/}
    </>
  );
}
