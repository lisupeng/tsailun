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
import Avatar from "@material-ui/core/Avatar";
import Button from "@material-ui/core/Button";
import CssBaseline from "@material-ui/core/CssBaseline";
import TextField from "@material-ui/core/TextField";
//import FormControlLabel from '@material-ui/core/FormControlLabel';
import Checkbox from "@material-ui/core/Checkbox";
import Link from "@material-ui/core/Link";
import Grid from "@material-ui/core/Grid";
import Box from "@material-ui/core/Box";
import LockOutlinedIcon from "@material-ui/icons/LockOutlined";
import Typography from "@material-ui/core/Typography";
import Container from "@material-ui/core/Container";
import Snackbar from "@material-ui/core/Snackbar";
import Alert from "@material-ui/core/Alert";
import { useHistory } from "react-router-dom";
import Utils from "./Utils";

import Lang from "../i18n/Lang";

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

export default function SignIn() {
  const [open, setOpen] = React.useState(false);
  const [msg, setMsg] = React.useState("Incorrect account or password.");

  const [keepLogin, setKeepLogin] = React.useState(true);

  const [uiAccount, setUiAccount] = React.useState("");

  const handleAccountInputChange = (event) => {
    setUiAccount(event.target.value);
  };

  const [uiPwd, setUiPwd] = React.useState("");

  const handlePwdInputChange = (event) => {
    setUiPwd(event.target.value);
  };

  let history = useHistory();

  const handleSubmit = (event) => {
    //event.preventDefault();
    //const formdata = new FormData(event.currentTarget);

    var account = uiAccount;
    var password = uiPwd;

    var url = "/auth";

    //var pwdUtf8Bytes = (new TextEncoder()).encode(password);

    Utils.clearSession();

    var hexstr_sha256 = Utils.SHA256(password);

    //console.log("str_sha256:"+hexstr_sha256);

    var uint8_arr = Utils.fromHex(hexstr_sha256);
    var credential = window.btoa(
      String.fromCharCode(...new Uint8Array(uint8_arr))
    );

    var postdata = { account, credential };
    // save page data and redirect to view page
    fetch(url, {
      method: "post",
      body: JSON.stringify(postdata),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // If keep login, save session to local storage
          if (keepLogin === true) {
            window.localStorage.session = res.session;
          } else {
            window.sessionStorage.session = res.session;
          }

          // If there's back url back to it, otherwise redirect to first page of default space
          var queryobj = Utils.parse(window.location.href);
          if (queryobj.back && queryobj.back.length > 0) {
            var backurl = decodeURIComponent(queryobj.back);
            history.push(backurl);
          } else if (res.space) {
            var user_space_root = res.space + "/pages";

            var url_listpage = user_space_root + "?op=listpage";

            // get path of first page of space
            fetch(url_listpage, {
              method: "post",
              body: JSON.stringify({ sid: Utils.getSessionId() }),
            })
              .then((response) => response.text())
              .then((data) => {
                var res = JSON.parse(data);
                if (res.status === "ok") {
                  if (
                    res.list &&
                    res.list.length > 0 &&
                    res.list[0] &&
                    res.list[0].pagedir
                  ) {
                    var first_page_url =
                      user_space_root + "/" + res.list[0].pagedir + "?op=view";
                    history.push(first_page_url);
                  } else if (res.list && res.list.length === 0) {
                    history.push(user_space_root);
                  }
                } else {
                  history.push(user_space_root);
                }
              })
              .catch((error) => {
                console.log("error is", error);
                history.push(user_space_root);
              });
          }
        } else {
          if (
            res.status === "fail" &&
            res.errcode === "username_or_pwd_incorrect"
          ) {
            setMsg("Incorrect account or password.");
            setOpen(true);
          } else {
            setMsg("Server error.");
            setOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleClose = (event, reason) => {
    if (reason === "clickaway") {
      return;
    }

    setOpen(false);
  };

  const keepLoginChange = (event) => {
    if (keepLogin === true) setKeepLogin(false);
    else setKeepLogin(true);
  };

  return (
    <Container component="main" maxWidth="xs">
      <CssBaseline />
      <Box
        sx={{
          marginTop: 8,
          display: "flex",
          flexDirection: "column",
          alignItems: "center",
        }}
      >
        <Avatar sx={{ m: 1, bgcolor: "secondary.main" }}>
          <LockOutlinedIcon />
        </Avatar>
        <Typography component="h1" variant="h5">
          {Lang.str_signin}
        </Typography>
        <Box sx={{ mt: 1 }}>
          <TextField
            margin="normal"
            required
            fullWidth
            id="signin_email"
            label={Lang.str_signin_email_or_account}
            name="signin_email"
            autoComplete="signin_email"
            onChange={handleAccountInputChange}
            autoFocus
          />
          <TextField
            margin="normal"
            required
            fullWidth
            name="signin_pwd"
            label={Lang.str_signin_password}
            type="password"
            id="signin_pwd"
            onChange={handlePwdInputChange}
            autoComplete="signin_pwd"
          />

          <Snackbar
            open={open}
            anchorOrigin={{ vertical: "top", horizontal: "center" }}
            autoHideDuration={2000}
            onClose={handleClose}
          >
            <Alert onClose={handleClose} severity="error">
              {" "}
              {msg}{" "}
            </Alert>
          </Snackbar>

          {/*
          <FormControlLabel
            control={<Checkbox checked={keepLogin} value="remember" onChange={keepLoginChange} color="primary" />}
            label={Lang.str_signin_remember_me}
          />*/}
          <Button
            fullWidth
            variant="contained"
            sx={{ mt: 3, mb: 2 }}
            onClick={handleSubmit}
          >
            {Lang.str_signin}
          </Button>
          <Grid container>
            <Grid item xs>
              <Link href="/spaces/default/pages" variant="body2">
                {Lang.str_signin_guest}
              </Link>
            </Grid>
            <Grid item>
              <Link href="/signup" variant="body2">
                {Lang.str_signin_donot_have_account}
              </Link>
            </Grid>
          </Grid>
        </Box>
      </Box>
      <Copyright sx={{ mt: 8, mb: 4 }} />
    </Container>
  );
}
