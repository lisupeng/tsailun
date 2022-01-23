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
import { experimentalStyled as styled } from "@material-ui/core/styles";
import CssBaseline from "@material-ui/core/CssBaseline";
import MuiDrawer from "@material-ui/core/Drawer";
import Box from "@material-ui/core/Box";
import MuiAppBar from "@material-ui/core/AppBar";
import Toolbar from "@material-ui/core/Toolbar";
//import List from '@material-ui/core/List';
import Typography from "@material-ui/core/Typography";
import Divider from "@material-ui/core/Divider";
import IconButton from "@material-ui/core/IconButton";
//import Badge from '@material-ui/core/Badge';
import Container from "@material-ui/core/Container";
import MenuIcon from "@material-ui/icons/Menu";
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
//import NotificationsIcon from '@material-ui/icons/Notifications';
import AdminListItems from "./listItems";
import AdminContentView from "./AdminContentView";

//import Globaldata from '../Globaldata';
import Utils from "../Utils";
import Menu from "@material-ui/core/Menu";
import MenuItem from "@material-ui/core/MenuItem";
import AccountCircle from "@material-ui/icons/AccountCircle";
import Lang from "../../i18n/Lang";

const drawerWidth = 240;

const AppBar = styled(MuiAppBar, {
  shouldForwardProp: (prop) => prop !== "open",
})(({ theme, open }) => ({
  zIndex: theme.zIndex.drawer + 1,
  transition: theme.transitions.create(["width", "margin"], {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
  ...(open && {
    marginLeft: drawerWidth,
    width: `calc(100% - ${drawerWidth}px)`,
    transition: theme.transitions.create(["width", "margin"], {
      easing: theme.transitions.easing.sharp,
      duration: theme.transitions.duration.enteringScreen,
    }),
  }),
}));

const Drawer = styled(MuiDrawer, {
  shouldForwardProp: (prop) => prop !== "open",
})(({ theme, open }) => ({
  "& .MuiDrawer-paper": {
    position: "relative",
    whiteSpace: "nowrap",
    width: drawerWidth,
    transition: theme.transitions.create("width", {
      easing: theme.transitions.easing.sharp,
      duration: theme.transitions.duration.enteringScreen,
    }),
    boxSizing: "border-box",
    ...(!open && {
      overflowX: "hidden",
      transition: theme.transitions.create("width", {
        easing: theme.transitions.easing.sharp,
        duration: theme.transitions.duration.leavingScreen,
      }),
      width: theme.spacing(7),
      [theme.breakpoints.up("sm")]: {
        width: theme.spacing(9),
      },
    }),
  },
}));

function UserInfo(props) {
  var name = Utils.getUserDisplayName();
  return <Typography variant="body2">{name}</Typography>;
}

function SignInMenuText(props) {
  var text;

  if (Utils.getSessionId() === "") text = Lang.str_sign_in;
  else text = Lang.str_sign_out;
  return <>{text}</>;
}

function DashboardContent() {
  let history = useHistory();

  const [open, setOpen] = React.useState(true);
  const toggleDrawer = () => {
    setOpen(!open);
  };

  // signin/out menu
  const [anchorElSignin, setAnchorElSignin] = React.useState(null);
  const menuSigninOpen = Boolean(anchorElSignin);

  const handleSignInMenu = (event) => {
    setAnchorElSignin(event.currentTarget);
  };

  const handleSignInMenuClose = () => {
    setAnchorElSignin(null);
  };

  const handleSignInMenuItemClicked = () => {
    // close menu
    setAnchorElSignin(null);

    Utils.clearSession();

    history.push("/signin");
  };

  return (
    <Box sx={{ display: "flex" }}>
      <CssBaseline />
      <AppBar position="absolute" open={open}>
        <Toolbar
          sx={{
            pr: "24px", // keep right padding when drawer closed
          }}
        >
          <IconButton
            edge="start"
            color="inherit"
            aria-label="open drawer"
            onClick={toggleDrawer}
            sx={{
              marginRight: "36px",
              ...(open && { display: "none" }),
            }}
          >
            <MenuIcon />
          </IconButton>
          <Typography
            component="h1"
            variant="h6"
            color="inherit"
            noWrap
            sx={{ flexGrow: 1 }}
          >
            {Lang.str_admin_admin_panel}
          </Typography>

          <IconButton
            aria-controls="menu-appbarSignnin"
            aria-haspopup="true"
            onClick={handleSignInMenu}
            edge="end"
            color="inherit"
          >
            <AccountCircle />
            <UserInfo />
          </IconButton>
        </Toolbar>

        <Menu
          id="menu-appbarSignnin"
          anchorEl={anchorElSignin}
          anchorOrigin={{
            vertical: "top",
            horizontal: "right",
          }}
          keepMounted
          transformOrigin={{
            vertical: "top",
            horizontal: "right",
          }}
          open={menuSigninOpen}
          onClose={handleSignInMenuClose}
        >
          <MenuItem onClick={handleSignInMenuItemClicked}>
            <SignInMenuText />
          </MenuItem>
        </Menu>
      </AppBar>
      <Drawer variant="permanent" open={open}>
        <Toolbar
          sx={{
            display: "flex",
            alignItems: "center",
            justifyContent: "flex-end",
            px: [1],
          }}
        >
          <IconButton onClick={toggleDrawer}>
            <ChevronLeftIcon />
          </IconButton>
        </Toolbar>
        <Divider />

        <AdminListItems />
      </Drawer>
      <Box
        component="main"
        sx={{
          backgroundColor: (theme) =>
            theme.palette.mode === "light"
              ? theme.palette.grey[100]
              : theme.palette.grey[900],
          flexGrow: 1,
          height: "100vh",
          overflow: "auto",
        }}
      >
        <Toolbar />
        <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
          <AdminContentView />
        </Container>
      </Box>
    </Box>
  );
}

export default function AdminDashboard() {
  return <DashboardContent />;
}
