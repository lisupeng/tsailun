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
import Button from "@material-ui/core/Button";

//
//import Grid from '@material-ui/core/Grid';
import Typography from "@material-ui/core/Typography";
//import TextField from '@material-ui/core/TextField';
//import FormControlLabel from '@material-ui/core/FormControlLabel';
//import Checkbox from '@material-ui/core/Checkbox';
import { OutlinedInput } from "@material-ui/core";

import MenuItem from "@material-ui/core/MenuItem";
import Select from "@material-ui/core/Select";

import Utils from "../Utils";
import Lang from "../../i18n/Lang";

const grayedOutInputStyle = {
  /*
    color: 'white',
    */
  backgroundColor: "lightgrey",
};

export default function UserEditView() {
  let history = useHistory();

  const [account, setAccount] = React.useState("");
  const [username, setUsername] = React.useState("");
  const [email, setEmail] = React.useState("");

  const [role, setRole] = React.useState("");

  const [spacelist, setSpacelist] = React.useState([]);

  const [space, setSpace] = React.useState("");

  const handleUsernameChange = (event) => {
    setUsername(event.target.value);
  };

  const handleEmailChange = (event) => {
    setEmail(event.target.value);
  };

  const handleRoleChange = (event) => {
    setRole(event.target.value);
  };

  const handleSpaceChange = (event) => {
    setSpace(event.target.value);
  };

  React.useEffect(() => {
    var queryobj = Utils.parse(window.location.href);
    if (!queryobj.account) return;

    var url = "/admin/users?op=getuserinfo&account=" + queryobj.account;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // get space list
          var url_listusr = "/admin/spaces?op=listspace";

          fetch(url_listusr, {
            method: "post",
            body: JSON.stringify({ sid: Utils.getSessionId() }),
          })
            .then((response) => response.text())
            .then((data) => {
              var res2 = JSON.parse(data);
              if (res2.status === "ok") {
                //var spacelist = [];

                if (res2.list) setSpacelist(res2.list);

                if (res.userinfo) {
                  if (res.userinfo.account) setAccount(res.userinfo.account);

                  if (res.userinfo.fullname) setUsername(res.userinfo.fullname);

                  if (res.userinfo.email) setEmail(res.userinfo.email);

                  if (res.userinfo.role) {
                    setRole(res.userinfo.role);
                  }

                  if (res.userinfo.space) {
                    setSpace(res.userinfo.space);
                  }
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

  const handleClickUpdateUser = (event) => {
    var url_updategroup = "/admin/users?op=updateuser";

    fetch(url_updategroup, {
      method: "post",
      body: JSON.stringify({
        sid: Utils.getSessionId(),
        account,
        fullname: username,
        email,
        role,
        space,
      }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          history.push("/admin/users");
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
            {Lang.str_admin_userview_account}
          </Typography>

          <OutlinedInput
            value={account}
            placeholder=""
            size="small"
            disabled
            style={grayedOutInputStyle}
          ></OutlinedInput>

          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_userview_name}
          </Typography>

          <OutlinedInput
            value={username}
            placeholder=""
            size="small"
            onChange={handleUsernameChange}
          ></OutlinedInput>

          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_userview_email}
          </Typography>

          <OutlinedInput
            value={email}
            placeholder=""
            size="small"
            onChange={handleEmailChange}
          ></OutlinedInput>
          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_userview_role}
          </Typography>

          <Select
            size="small"
            value={role}
            displayEmpty
            inputProps={{ "aria-label": "Without label" }}
            onChange={handleRoleChange}
          >
            <MenuItem value={"regular"}>Regular</MenuItem>
            <MenuItem value={"guest"}>Guest</MenuItem>
            <MenuItem value={"admin"}>Admin</MenuItem>
          </Select>
          <p />

          <Typography variant="subtitle2" display="block" gutterBottom>
            {Lang.str_admin_userview_userspace}
          </Typography>

          <Select
            size="small"
            value={space}
            displayEmpty
            inputProps={{ "aria-label": "Without label" }}
            onChange={handleSpaceChange}
          >
            {spacelist.map((space) => (
              <MenuItem value={space.spacename}>{space.spacename}</MenuItem>
            ))}
          </Select>

          <p />
        </Paper>

        <p />
        <Button
          variant="contained"
          size="small"
          onClick={(event) => handleClickUpdateUser(event)}
        >
          {Lang.str_dlg_btn_save}
        </Button>
      </>
    </React.Fragment>
  );
}
