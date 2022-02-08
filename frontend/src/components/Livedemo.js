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
import Utils from "./Utils";

import Lang from "../i18n/Lang";

import Dialog from "@material-ui/core/Dialog";
import DialogActions from "@material-ui/core/DialogActions";
import DialogContent from "@material-ui/core/DialogContent";
import DialogContentText from "@material-ui/core/DialogContentText";
import DialogTitle from "@material-ui/core/DialogTitle";
import Paper from "@material-ui/core/Paper";

export default function Livedemo() {
  const [ifCreateAccountDlgOpen, setIfCreateAccountDlgOpen] =
    React.useState(false);
  const [demoAccountDlgOpen, setDemoAccountDlgOpen] = React.useState(false);
  const [account, setAccount] = React.useState("");

  let history = useHistory();

  const autoCreateAccount = () => {
    // if already has session storage, redirect to user's space
    if (window.localStorage.session) {
      var sessionObj = JSON.parse(window.localStorage.session);
      history.push(sessionObj.default_spacepath + "/pages");
      return;
    }

    // create user and space for demo
    var url = "/demo?op=createdemo";

    fetch(url)
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          window.localStorage.session = res.session;

          var sessionObj = JSON.parse(res.session);
          setAccount(sessionObj.account);

          // show demo account
          setDemoAccountDlgOpen(true);
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  React.useEffect(() => {
    // if already has session storage, redirect to user's space
    if (window.localStorage.session) {
      var sessionObj = JSON.parse(window.localStorage.session);
      history.push(sessionObj.default_spacepath + "/pages");
      return;
    }

    setIfCreateAccountDlgOpen(true);
  }, []);

  const handleCloseIfCreateAccountDlg = () => {
    setIfCreateAccountDlgOpen(false);
  };

  const handleCloseDemoAccountDlg = () => {
    setDemoAccountDlgOpen(false);

    // redirect to demo space
    if (window.localStorage.session) {
      var sessionObj = JSON.parse(window.localStorage.session);
      history.push(sessionObj.default_spacepath + "/pages");
      return;
    }
  };

  function alreadyHaveAccountClicked(event) {
    event.preventDefault();
    history.push("/signin");
  }

  function createAccountClicked(event) {
    event.preventDefault();
    autoCreateAccount();
  }

  return (
    <>
      <Dialog
        open={demoAccountDlgOpen}
        onClose={handleCloseDemoAccountDlg}
        aria-labelledby="form-dialog-title"
        fullWidth={true}
        maxWidth={"sm"}
      >
        <DialogTitle id="form-dialog-title">
          {Lang.str_dlg_demo_account_title}
        </DialogTitle>
        <DialogContent>
          <Paper
            elevation={0}
            sx={{
              display: "flex",
              flexDirection: "column",
            }}
          >
            <Typography variant="subtitle1" component="div">
              {Lang.str_dlg_demo_account + account}
            </Typography>
            <Typography variant="subtitle1" component="div">
              {Lang.str_dlg_demo_pwd + "123"}
            </Typography>
          </Paper>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCloseDemoAccountDlg} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
          <Button onClick={handleCloseDemoAccountDlg} color="primary">
            {Lang.str_dlg_btn_cancel}
          </Button>
        </DialogActions>
      </Dialog>

      <Dialog
        open={ifCreateAccountDlgOpen}
        onClose={autoCreateAccount}
        aria-labelledby="form-dialog-title2"
        fullWidth={true}
        maxWidth={"sm"}
      >
        <DialogTitle id="form-dialog-title2">
          {Lang.str_dlg_create_title}
        </DialogTitle>
        <DialogContent>
          <Paper
            elevation={0}
            sx={{
              display: "flex",
              flexDirection: "column",
            }}
          >
            <Typography variant="subtitle1" component="div">
              <Link color="primary" href="#" onClick={createAccountClicked}>
                {Lang.str_dlg_create_for_me}
              </Link>
            </Typography>

            <Typography variant="subtitle1" component="div">
              <Link
                color="primary"
                href="#"
                onClick={alreadyHaveAccountClicked}
              >
                {Lang.str_dlg_already_have_account}
              </Link>
            </Typography>
          </Paper>
        </DialogContent>
        <DialogActions>
          <Button onClick={autoCreateAccount} color="primary">
            {Lang.str_dlg_btn_ok}
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
}
