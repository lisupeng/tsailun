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
import FormControlLabel from "@material-ui/core/FormControlLabel";
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

import Lang from "../i18n/Lang";

import Utils from "./Utils";

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

export default function SignUp() {
  const [open, setOpen] = React.useState(false);
  const [msg, setMsg] = React.useState("");
  const [sev, setSev] = React.useState("error");

  const [uiFullname, setUiFullname] = React.useState("");
  const [uiEmail, setUiEmail] = React.useState("");
  const [uiPwd, setUiPwd] = React.useState("");
  const [uiConfirmPwd, setUiConfirmPwd] = React.useState("");

  const handleFullnameInputChange = (event) => {
    setUiFullname(event.target.value);
  };
  const handleEmailInputChange = (event) => {
    setUiEmail(event.target.value);
  };
  const handlePwdInputChange = (event) => {
    setUiPwd(event.target.value);
  };
  const handleConfirmPwdInputChange = (event) => {
    setUiConfirmPwd(event.target.value);
  };

  let history = useHistory();

  const handleClose = (event, reason) => {
    if (reason === "clickaway") {
      return;
    }

    setOpen(false);
  };

  const handleSubmit = (event) => {
    //event.preventDefault();
    // eslint-disable-next-line no-console

    var password = uiPwd;
    var confirmpwd = uiConfirmPwd;
    var email = uiEmail;
    var fullname = uiFullname;

    // check if input empty
    if (
      !fullname ||
      fullname.length === 0 ||
      !email ||
      email.length === 0 ||
      !password ||
      password.length === 0
    ) {
      setSev("error");
      setMsg("Please fill mandatory field.");
      setOpen(true);
      return;
    }

    if (password !== confirmpwd) {
      setSev("error");
      setMsg("Password inconsistent.");
      setOpen(true);
      return;
    }

    var hexstr_sha256 = Utils.SHA256(password);

    var uint8_arr = Utils.fromHex(hexstr_sha256);
    var credential = window.btoa(
      String.fromCharCode(...new Uint8Array(uint8_arr))
    );

    var postdata = { email, fullname, credential };
    // save page data and redirect to view page
    var url = "/signup?op=register";
    fetch(url, {
      method: "post",
      body: JSON.stringify(postdata),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // signup success
          setSev("success");
          setMsg("Sign up successfully.");
          setOpen(true);

          setTimeout(() => {
            history.push("/signin");
          }, 3000);
        } else {
          if (res.status === "fail" && res.errcode === "email_already_exists") {
            setSev("error");
            setMsg("Email already in use, unable to create account.");
            setOpen(true);
          } else if (
            res.status === "fail" &&
            res.errcode === "user_count_exceeds_limit"
          ) {
            setSev("error");
            setMsg("User account exceeds limit.");
            setOpen(true);
          } else {
            setSev("error");
            setMsg("Server error.");
            setOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));

    //});
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
          {Lang.str_signup}
        </Typography>

        <Snackbar
          open={open}
          anchorOrigin={{ vertical: "top", horizontal: "center" }}
          autoHideDuration={2000}
          onClose={handleClose}
        >
          <Alert onClose={handleClose} severity={sev}>
            {" "}
            {msg}{" "}
          </Alert>
        </Snackbar>

        <Box sx={{ mt: 3 }}>
          <Grid container spacing={2}>
            <Grid item xs={12}>
              <TextField
                autoComplete="userfullname"
                name="userfullname"
                required
                fullWidth
                id="userfullname"
                label={Lang.str_signup_name}
                onChange={handleFullnameInputChange}
                autoFocus
              />
            </Grid>

            <Grid item xs={12}>
              <TextField
                required
                fullWidth
                id="signup_email"
                label={Lang.str_email_addr}
                name="signup_email"
                onChange={handleEmailInputChange}
                autoComplete="signup_email"
              />
            </Grid>
            <Grid item xs={12}>
              <TextField
                required
                fullWidth
                name="signup_pwd"
                label={Lang.str_signin_password}
                type="password"
                id="signup_pwd"
                onChange={handlePwdInputChange}
              />
            </Grid>

            <Grid item xs={12}>
              <TextField
                required
                fullWidth
                name="signup_cfmpwd"
                label={Lang.str_signup_confirm_password}
                type="password"
                onChange={handleConfirmPwdInputChange}
                id="signup_cfmpwd"
              />
            </Grid>

            <Grid item xs={12}>
              <FormControlLabel
                control={<Checkbox value="allowExtraEmails" color="primary" />}
                label={Lang.str_signup_accept_email}
              />
            </Grid>
          </Grid>
          <Button
            fullWidth
            variant="contained"
            sx={{ mt: 3, mb: 2 }}
            onClick={handleSubmit}
          >
            {Lang.str_btn_signup}
          </Button>
          <Grid container justifyContent="flex-end">
            <Grid item>
              <Link href="/signin" variant="body2">
                {Lang.str_already_have_account}
              </Link>
            </Grid>
          </Grid>
        </Box>
      </Box>
      <Copyright sx={{ mt: 5 }} />
    </Container>
  );
}
