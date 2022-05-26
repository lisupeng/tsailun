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

import * as React from "react";
import { useHistory } from "react-router-dom";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import Typography from "@material-ui/core/Typography";
import Link from "@material-ui/core/Link";
import Utils from "../Utils";

function Copyright(props) {
  return (
    <Typography
      variant="body2"
      color="text.secondary"
      align="center"
      {...props}
    >
      {"Copyright © "}
      <Link color="inherit" href="http://www.tsailun.net/">
        TSAILUN
      </Link>{" "}
      {new Date().getFullYear()}
      {"."}
    </Typography>
  );
}

export default function StatsView() {
  let history = useHistory();

  const [uptime, setUptime] = React.useState("0 days, 0 hours, 0 minutes");  
  const [reqcount, setReqcount] = React.useState("0");
  const [sessioncount, setSessioncount] = React.useState("0");
  const [createpg, setCreatepg] = React.useState("0");
  const [readpg, setReadpg] = React.useState("0");
  const [modifypg, setModifypg] = React.useState("0");
  const [uploadfile, setUploadfile] = React.useState("0");
  const [delfile, setDelfile] = React.useState("0");
  
  const updateStatsData = () => {
    var url_listusr = "/admin?op=getstats";

    fetch(url_listusr, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.stats) {
            setUptime(res.stats.up_days+" days, "+res.stats.up_hours+" hours, "+res.stats.up_mins+" minutes");
            setReqcount(res.stats.ReqCounter);
            setSessioncount(res.stats.sessionCounter);
            setCreatepg(res.stats.createPage);
            setReadpg(res.stats.readPage);
            setModifypg(res.stats.writePage);
            setUploadfile(res.stats.uploadFile);
            setDelfile(res.stats.delFile);
          }

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
  
  React.useEffect(() => {
    updateStatsData();
  }, []);
  
  return (
    <>
      {"Server uptime: "+uptime} <p> </p>
      {"Request Count: "+reqcount} <p> </p>
      {"Session Count: "+sessioncount} <p> </p>
      {"Page Creation: "+createpg} <p> </p>
      {"Page Read: "+readpg} <p> </p>
      {"Page Modification: "+modifypg} <p> </p>
      {"File Upload: "+uploadfile} <p> </p>
      {"File Deletion: "+delfile} <p> </p>
    </>
  );
}
