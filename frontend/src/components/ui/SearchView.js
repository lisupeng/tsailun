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
//import MuiDrawer from '@material-ui/core/Drawer';
import Box from "@material-ui/core/Box";
import MuiAppBar from "@material-ui/core/AppBar";
import Toolbar from "@material-ui/core/Toolbar";
//import List from '@material-ui/core/List';
import Typography from "@material-ui/core/Typography";
//import Divider from '@material-ui/core/Divider';
import IconButton from "@material-ui/core/IconButton";
//import Badge from '@material-ui/core/Badge';
import Container from "@material-ui/core/Container";
//import Grid from '@material-ui/core/Grid';
//import Paper from '@material-ui/core/Paper';
import Link from "@material-ui/core/Link";
//import MenuIcon from '@material-ui/icons/Menu';
//import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
//import NotificationsIcon from '@material-ui/icons/Notifications';
//import AdminListItems from './listItems';
//import Chart from './Chart';
//import Deposits from './Deposits';
//import Orders from './Orders';

//import AdminContentView from './AdminContentView';

import Utils from "../Utils";
import Menu from "@material-ui/core/Menu";
import MenuItem from "@material-ui/core/MenuItem";
import AccountCircle from "@material-ui/icons/AccountCircle";

import { InputAdornment, OutlinedInput } from "@material-ui/core";
import SearchIcon from "@material-ui/icons/Search";

import GlobalVarMgr from "../GlobalVarMgr";
import Lang from "../../i18n/Lang";

var searchInput = "";

const AppBar = styled(MuiAppBar, {
  shouldForwardProp: (prop) => prop !== "open",
})(({ theme, open }) => ({
  zIndex: theme.zIndex.drawer + 1,
}));

const searchBoxStyle = {
  /*
    color: 'white',
    */
  backgroundColor: "white",
};

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

export default function SearchView() {
  let history = useHistory();

  const [searchResList, setSearchResList] = React.useState([]);
  const [searchState, setSearchState] = React.useState("not_start");

  // signin/out menu
  const [anchorElSignin, setAnchorElSignin] = React.useState(null);
  const menuSigninOpen = Boolean(anchorElSignin);

  const doSearch = () => {
    if (GlobalVarMgr.getSearchInput().length === 0) return;

    var url_search =
      "/admin/search?op=search&s=" + GlobalVarMgr.getSearchInput();

    setSearchState("searching");

    fetch(url_search, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          setSearchState("done");

          if (res.searchres) setSearchResList(res.searchres);
        }
      })
      .catch((error) => console.log("error is", error));
  };

  React.useEffect(() => {
    //console.log("searchInput="+GlobalVarMgr.getSearchInput());

    doSearch();
  }, []);

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

  const handleSearchInputChange = (event) => {
    searchInput = event.target.value;
    GlobalVarMgr.setSearchInput(searchInput);
  };

  const handleSearchIconClicked = () => {
    //console.log("search input:"+searchInput);

    if (searchInput.length > 0) {
      GlobalVarMgr.setSearchInput(searchInput);

      doSearch();
    }
  };

  const onKeyPress = (e) => {
    if (e.key === "Enter") {
      handleSearchIconClicked();
      e.preventDefault();
    }
  };

  function RenderSearchState() {
    if (searchState === "searching") {
      return (
        <>
          <Typography variant="h5">{"Searching..."}</Typography>
        </>
      );
    } else {
      return <></>;
    }
  }

  function RenderSearchResults() {
    if (searchState === "done") {
      if (searchResList.length === 0) {
        return (
          <>
            <Typography variant="h5">{Lang.str_search_no_match}</Typography>
          </>
        );
      } else {
        return (
          <>
            {searchResList.map((entry) => (
              <div key={entry.path}>
                <Link href={entry.path} variant="h5">
                  {Utils.truncateStrForDisplay(entry.title, 60)}
                </Link>
                <Typography variant="body2" style={{ color: "#296b21" }}>
                  {Utils.truncateStrForDisplay(
                    window.location.origin + entry.path,
                    80
                  )}
                </Typography>

                <Typography variant="body2" style={{ whiteSpace: "pre-line" }}>
                  {"\n\n"}
                </Typography>
              </div>
            ))}
          </>
        );
      }
    } else {
      return <></>;
    }
  }

  return (
    <Box sx={{ display: "flex" }}>
      <CssBaseline />
      <AppBar position="absolute">
        <Toolbar
          sx={{
            pr: "24px", // keep right padding when drawer closed
          }}
        >
          <Typography
            component="h1"
            variant="h6"
            color="inherit"
            noWrap
            sx={{ flexGrow: 1 }}
          >
            {Lang.str_search_search_results}
          </Typography>

          <OutlinedInput
            placeholder=""
            size="small"
            style={searchBoxStyle}
            onChange={handleSearchInputChange}
            onKeyPress={onKeyPress}
            defaultValue={GlobalVarMgr.getSearchInput()}
            endAdornment={
              <InputAdornment position="end">
                <IconButton
                  size="small"
                  edge="start"
                  onClick={handleSearchIconClicked}
                >
                  <SearchIcon />
                </IconButton>
              </InputAdornment>
            }
          ></OutlinedInput>

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

      <Box
        component="main"
        sx={{
          flexGrow: 1,
          height: "100vh",
          overflow: "auto",
        }}
      >
        <Toolbar />
        <Container maxWidth="lg" sx={{ mt: 4, mb: 4, ml: 9 }}>
          <RenderSearchState />

          <RenderSearchResults />
        </Container>
      </Box>
    </Box>
  );
}
