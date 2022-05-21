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
//import Divider from '@material-ui/core/Divider';
import IconButton from "@material-ui/core/IconButton";
//import Badge from '@material-ui/core/Badge';
import Container from "@material-ui/core/Container";
//import Grid from '@material-ui/core/Grid';
//import Paper from '@material-ui/core/Paper';
//import Link from '@material-ui/core/Link';
import MenuIcon from "@material-ui/icons/Menu";
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import AddIcon from '@material-ui/icons/Add';
import CloseIcon from '@material-ui/icons/Close';

//import NotificationsIcon from '@material-ui/icons/Notifications';
import MoreIcon from "@material-ui/icons/MoreVert";
//import { mainListItems, secondaryListItems } from './listItems';
//import Chart from './Chart';
//import Deposits from './Deposits';
//import Orders from './Orders';
import CAntdtree from "./CAntdtree";
import Menu from "@material-ui/core/Menu";
import MenuItem from "@material-ui/core/MenuItem";
import AccountCircle from "@material-ui/icons/AccountCircle";
import Contentarea from "./Contentarea";

import Button from "@material-ui/core/Button";
import TextField from "@material-ui/core/TextField";
import Dialog from "@material-ui/core/Dialog";
import DialogActions from "@material-ui/core/DialogActions";
import DialogContent from "@material-ui/core/DialogContent";
import DialogContentText from "@material-ui/core/DialogContentText";
import DialogTitle from "@material-ui/core/DialogTitle";
import Globaldata from "../Globaldata";
import Utils from "../Utils";

import { InputAdornment, OutlinedInput } from "@material-ui/core";
import SearchIcon from "@material-ui/icons/Search";

import Snackbar from "@material-ui/core/Snackbar";
import Alert from "@material-ui/core/Alert";
import Paper from "@material-ui/core/Paper";

import GlobalVarMgr from "../GlobalVarMgr";

import Lang from "../../i18n/Lang";
import Select from "@material-ui/core/Select";
import Link from "@material-ui/core/Link";

import FormControlLabel from "@material-ui/core/FormControlLabel";
import FormLabel from "@material-ui/core/FormLabel";
import Radio from "@material-ui/core/Radio";
import RadioGroup from "@material-ui/core/RadioGroup";
import FormControl from "@material-ui/core/FormControl";

import Slide from '@material-ui/core/Slide';

const Transition = React.forwardRef(function Transition(props, ref) {
  return <Slide direction="up" ref={ref} {...props} />;
});

var searchInput = "";

const searchBoxStyle = {
  /*
    color: 'white',
    */
  backgroundColor: "white",
  width: "200px"
};

function UserInfo(props) {
  var name = Utils.getUserDisplayName();

  if (!Utils.windowIsNarrow()) {
    return (
      <Typography variant="body2">
        {Utils.truncateStrForDisplay(name, 20)}
      </Typography>
    );
  } else {
    return <></>;
  }
}

function RenderProdName() {
  if (!Utils.windowIsNarrow()) {
    return (
      <Typography
        component="h1"
        variant="h6"
        color="inherit"
        noWrap
        sx={{ flexGrow: 1 }}
      >
        {"TSAILUN"}
      </Typography>
    );
  } else {
    return (
      <Typography
        component="h1"
        variant="h6"
        color="inherit"
        noWrap
        sx={{ flexGrow: 1 }}
      >
        {" "}
      </Typography>
    );
  }
}

function SignInMenuText(props) {
  var text;

  if (Utils.getSessionId() === "") text = Lang.str_sign_in;
  else text = Lang.str_sign_out;
  return <>{text}</>;
}

function DelPageDlgText(props) {
  var text;

  var pagetitle = Globaldata.titleOfCurDisplayedPage;

  text = Utils.sprint(Lang.str_dlg_delpage_msg, pagetitle);

  return <>{text}</>;
}

function getTreeViewWidth()
{
    if (Utils.windowIsNarrow())
        return 240;
    else
        return 325;
}

const initialDrawerWidth = getTreeViewWidth();

function jsonpReq(url, id) {
    var script = window.document.getElementById(id);
    if (script) {
        window.document.body.removeChild(script);
    }

    script = window.document.createElement("script");
    script.setAttribute("src", url);
    script.setAttribute("id",id);

    window.document.body.appendChild(script);
}

// function CheckUpdateCB

function __CheckUpdate() {
    var date = new Date();
    var d = date.getDate();
    
    if (d == GlobalVarMgr.getLastCheckupdateDay())
    {
        return;
    }
    
    GlobalVarMgr.setLastCheckupdateDay(d);
    
    var rn=Utils.genRand(0, 100000);
    
    var url = "http://tsailun.com.cn/api?op=checkupdate&rn="+rn;
    
    jsonpReq(url, "id_script_chkupdate");
}

const AppBar = styled(MuiAppBar, {
  shouldForwardProp: (prop) => prop !== "open",
})(({ theme, open }) => ({
  zIndex: theme.zIndex.drawer + 1,
  transition: theme.transitions.create(["width", "margin"], {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
}));

var currentDrawerWidth = initialDrawerWidth;

const Drawer = styled(MuiDrawer, {
  shouldForwardProp: (prop) => prop !== "open",
})(({ theme, open }) => ({
  "& .MuiDrawer-paper": {
    background: "#ffffff",
    position: "relative",
    whiteSpace: "nowrap",
    width: currentDrawerWidth,
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
      width: theme.spacing(0),
      /*[theme.breakpoints.up('sm')]: {
          width: theme.spacing(9),
        },*/
    }),
  },
}));

function MainViewContent() {
  let history = useHistory();
  const [open, setOpen] = React.useState(!Utils.windowIsNarrow());
  const [msgBarOpen, setMsgBarOpen] = React.useState(false);
  const [errmsg, setErrMsg] = React.useState("Access denied.");
  const [sev, setSev] = React.useState("error");
  const [chpwd_dlg_errmsg, setChpwd_dlg_errmsg] = React.useState("");

  const [drawerWidth, setDrawerWidth] = React.useState(initialDrawerWidth);
  const [splitterPos, setSplitterPos] = React.useState(drawerWidth);

  const [pageLocation, setPageLocation] = React.useState(
    Lang.str_createpagedlg_putpageafter
  );

  const [userReadableSpaces, setUserReadableSpaces] = React.useState([]);
  
  const [newpgDlgErrmsg, setNewpgDlgErrmsg] = React.useState("");

  const toggleDrawer = () => {
    setOpen(!open);
  };

  const [anchorEl, setAnchorEl] = React.useState(null);
  const menuopen = Boolean(anchorEl);

  // signin/out menu
  const [anchorElSignin, setAnchorElSignin] = React.useState(null);
  const menuSigninOpen = Boolean(anchorElSignin);
  
  // tree context menu
  //const [anchorTreeContextMenu, setAnchorTreeContextMenu] = React.useState(null);
  const [contextMenuX, setContextMenuX] = React.useState(0);
  const [contextMenuY, setContextMenuY] = React.useState(0);
  const [treeContextMenuOpen, setTreeContextMenuOpen] = React.useState(false);

  const handleTreeContextMenuClose = () => {
    setTreeContextMenuOpen(false);
  };

  /* DOM did mount */
  React.useEffect(() => {
    var url = "/spaces?op=listuserspace";

    // get user readable space list
    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok" && res.list) {
          setUserReadableSpaces(res.list);
        } else {
          if (res.errcode === "invalid_session") {
          }
        }
      })
      .catch((error) => console.log("error is", error));
      
    __CheckUpdate();

  }, []);

  const handleMenu = (event) => {
    setAnchorEl(event.currentTarget);
  };

  const handleSignInMenu = (event) => {
    setAnchorElSignin(event.currentTarget);
  };

  const handleMenuClose = () => {
    setAnchorEl(null);
  };

  const handleSignInMenuClose = () => {
    setAnchorElSignin(null);
  };

  const handleCreateNewPageMenuClicked = () => {
    // close menu
    setAnchorEl(null);

    //console.log("selected: "+Globaldata.selectedTreeNode);

    // check permission
    var url_check_writepermission =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages?op=checkpermission&access=writespace";

    fetch(url_check_writepermission, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // show dlg
          setNewpgDlgErrmsg("");
          setNewPageDlgOpen(true);
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            this.props.history.push("/signin");
          } else if (res.errcode === "access_denied") {
            // setMsg
            setErrMsg("Access denied.");
            setSev("error");
            // setOpen
            setMsgBarOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };
  
  const handleNewPageContextMenuItemClicked = () => {
      setTreeContextMenuOpen(false);
      
      setPageLocation(Lang.str_createpagedlg_putpageafter);
      
      handleCreateNewPageMenuClicked();
  };
  
  const handleNewChildPageContextMenuItemClicked = () => {
      setTreeContextMenuOpen(false);
      
      setPageLocation(Lang.str_createpagedlg_putpagechild);
      
      handleCreateNewPageMenuClicked();
  };

  const handlePageHistoryMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    /*
    var nodeid = Globaldata.selectedTreeNode;
    if (!nodeid || nodeid === '')
        return;
    var path = Utils.getNodePathByNodeId(Globaldata.treeInstance.state.treeData, nodeid);
    */

    // redirect to history view page
    //var url = Utils.getSpacePathFromUrl(window.location.pathname)+'/pages'+'?op=history';
    var url = window.location.pathname + "?op=history";
    history.push(url);
  };

  const handleDelPageMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    // check permission
    // webapi to delete
    var url_check_writepermission =
      window.location.pathname + "?op=checkpermission&access=writespace";
    console.log("url_check_writepermission:" + url_check_writepermission);

    fetch(url_check_writepermission, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // show delete page dialog
          setDelPageDlgOpen(true);
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            this.props.history.push("/signin");
          } else if (res.errcode === "access_denied") {
            // setMsg
            setErrMsg("Access denied.");
            setSev("error");
            // setOpen
            setMsgBarOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleExportPageMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    // TODO check permission

    setExportPageDlgOpen(true);
  };

  // Rename Page dialog
  const [renamePageDlgOpen, setRenamePageDlgOpen] = React.useState(false);

  const handleCloseRenamePageDlg = () => {
    setRenamePageDlgOpen(false);
  };

  const [renamedTitle, setRenamedTitle] = React.useState("");

  const handleRenameTitleInputChange = (event) => {
    setRenamedTitle(event.target.value);
  };

  const handleRenamePageMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    // check if a page selected
    if (
      !(
        typeof Globaldata.selectedTreeNode === "string" &&
        Globaldata.selectedTreeNode.length > 0
      )
    ) {
      // setMsg
      setErrMsg("No page selected.");
      setSev("error");

      // setOpen
      setMsgBarOpen(true);

      return;
    }

    // check permission
    var url_check_writepermission =
      window.location.pathname + "?op=checkpermission&access=writespace";
    console.log("url_check_writepermission:" + url_check_writepermission);

    fetch(url_check_writepermission, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // show rename page dialog
          setRenamePageDlgOpen(true);
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            this.props.history.push("/signin");
          } else if (res.errcode === "access_denied") {
            // setMsg
            setErrMsg("Access denied.");
            setSev("error");
            // setOpen
            setMsgBarOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleRenamePage = () => {
    if (renamedTitle.length === 0) {
      return;
    }

    var path = "";

    // if no page selected, close dlg and return. Shouldn't happen, already checked this in handleRenamePageMenuItemClicked()
    if (
      !(
        typeof Globaldata.selectedTreeNode === "string" &&
        Globaldata.selectedTreeNode.length > 0
      )
    ) {
      setRenamePageDlgOpen(false);
      return;
    }

    var nodeid = Globaldata.selectedTreeNode;
    path = Utils.getNodePathByNodeId(
      Globaldata.treeInstance.state.treeData,
      nodeid
    );
    // length at least should be 2 ('/a')
    if (path.length < 2) {
      setRenamePageDlgOpen(false);
      return;
    }

    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      path +
      "?op=rename&title=" +
      renamedTitle;

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // set data to notify tree update
          Globaldata.treeDataShouldUpdate = "true";
          Globaldata.treeDataUpdateReason = "renamepage";

          // refresh parent of selected node
          var keypath = Utils.getKeyPathOfNode(
            Globaldata.treeInstance.state.treeData,
            Globaldata.selectedTreeNode
          );
          var keylist = keypath.split("/");

          var parentNodeKey;
          if (keylist.length >= 2) parentNodeKey = keylist[keylist.length - 2];
          else parentNodeKey = "";

          Globaldata.nodeToRefresh = parentNodeKey;

          // for tree node selection
          Globaldata.preferredSelecKey = "";

          Globaldata.invalidatePageCache = true;

          var pgdir = res.pagedir;
          //var url_edit = Utils.buildUrlForEdit(Utils.getSpacePathFromUrl(window.location.pathname), path, pgdir, loc);
          history.push(window.location.pathname + "?op=view");
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));

    // close dlg
    setRenamePageDlgOpen(false);
  };

  // handleAttachmentMenuItemClicked
  const handleAttachmentMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    var url = window.location.pathname + "?op=attachment";
    history.push(url);
  };
  // change space
  const [changeSpaceDlgOpen, setChangeSpaceDlgOpen] = React.useState(false);

  const handleCloseChangeSpaceDlg = () => {
    setChangeSpaceDlgOpen(false);
  };

  const handleChangeSpaceMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);
    setChangeSpaceDlgOpen(true);
  };

  const handleMovePageUpItemClicked = () => {
    // close menu
    setAnchorEl(null);

    var nodeid = Globaldata.selectedTreeNode;
    if (!nodeid) return;
    var path = Utils.getNodePathByNodeId(
      Globaldata.treeInstance.state.treeData,
      nodeid
    );

    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      path +
      "?op=moveup";

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // set data to notify tree update
          Globaldata.treeDataShouldUpdate = "true";
          Globaldata.treeDataUpdateReason = "moveup";

          var url =
            Utils.getSpacePathFromUrl(window.location.pathname) +
            "/pages" +
            path;

          history.push(url);
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleMovePageDownItemClicked = () => {
    // close menu
    setAnchorEl(null);

    var nodeid = Globaldata.selectedTreeNode;
    if (!nodeid) return;
    var path = Utils.getNodePathByNodeId(
      Globaldata.treeInstance.state.treeData,
      nodeid
    );

    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      path +
      "?op=movedown";

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // set data to notify tree update
          Globaldata.treeDataShouldUpdate = "true";
          Globaldata.treeDataUpdateReason = "moveup";

          var url =
            Utils.getSpacePathFromUrl(window.location.pathname) +
            "/pages" +
            path;

          history.push(url);
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleAdminMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    var url_check_adminpermission = "/admin?op=checkpermission&access=sysadmin";

    fetch(url_check_adminpermission, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          history.push("/admin");
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            this.props.history.push("/signin");
          } else if (res.errcode === "access_denied") {
            // setMsg
            setErrMsg("E1002 "+Lang.str_err_sigin_as_admin);
            setSev("error");
            // setOpen
            setMsgBarOpen(true);
          }
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleSignInMenuItemClicked = () => {
    // close menu
    setAnchorElSignin(null);

    Utils.clearSession();

    history.push("/signin");
  };

  const handleChangePwdMenuItemClicked = () => {
    // close menu
    setAnchorElSignin(null);

    setChangePwdDlgOpen(true);
  };

  function ChangeUserPwdMenuItem(props) {
    if (Utils.getSessionId() === "") {
      return <></>;
    } else {
      return (
        <>
          <MenuItem onClick={handleChangePwdMenuItemClicked}>
            {Lang.str_menu_change_password}
          </MenuItem>
        </>
      );
    }
  }

  // Create New Page dialog
  const [newPageDlgOpen, setNewPageDlgOpen] = React.useState(false);

  const handleCloseNewPageDlg = () => {
    setNewPageDlgOpen(false);
  };

  const [pageTitle, setPageTitle] = React.useState("");

  const handlePageTitleInputChange = (event) => {
    setPageTitle(event.target.value);
  };

  const handleCreateNewPage = () => {
    if (pageTitle.length === 0) {
      return;
    }
    
    var re = /\%/;
    if (re.test(pageTitle)) {
    //if (pageTitle.contains('%')) {
        setNewpgDlgErrmsg("Title could not contain '%'");
        return;
    }

    var path = "";
    // check if node selected
    if (
      typeof Globaldata.selectedTreeNode === "string" &&
      Globaldata.selectedTreeNode.length > 0
    ) {
      var nodeid = Globaldata.selectedTreeNode;
      path = Utils.getNodePathByNodeId(
        Globaldata.treeInstance.state.treeData,
        nodeid
      );
      //console.dir(Globaldata.treeInstance);
    }

    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      path +
      "?op=create&title=" +
      encodeURIComponent(pageTitle);

    // append location param
    var loc = "";
    if (path.length > 0) {
      if (pageLocation === Lang.str_createpagedlg_putpageafter) {
        loc = "after";
      } else if (pageLocation === Lang.str_createpagedlg_putpagebefore) {
        loc = "before";
      } else if (pageLocation === Lang.str_createpagedlg_putpagechild) {
        loc = "child";
      } else if (pageLocation === Lang.str_createpagedlg_putpageroot) {
        loc = "root";
      }
    }

    if (loc.length > 0) {
      url = url + "&loc=" + loc;
    }

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // set data to notify tree update
          Globaldata.treeDataShouldUpdate = "true";
          Globaldata.treeDataUpdateReason = "createpage";

          if (loc === "before" || loc === "after") {
            // refresh parent of selected node
            var keypath = Utils.getKeyPathOfNode(
              Globaldata.treeInstance.state.treeData,
              Globaldata.selectedTreeNode
            );
            var keylist = keypath.split("/");

            var parentNodeKey;
            if (keylist.length >= 2)
              parentNodeKey = keylist[keylist.length - 2];
            else parentNodeKey = "";

            Globaldata.nodeToRefresh = parentNodeKey;
          } else if (loc === "child") {
            // refresh selected node
            Globaldata.nodeToRefresh = Globaldata.selectedTreeNode;
          } else {
            // refresh entire tree
            Globaldata.nodeToRefresh = "";
          }

          // for tree node selection
          Globaldata.preferredSelecKey = res.uid;

          var pgdir = res.pagedir;
          var url_edit = Utils.buildUrlForEdit(
            Utils.getSpacePathFromUrl(window.location.pathname),
            path,
            pgdir,
            loc
          );
          history.push(url_edit);
        } else {
            if (res && res.errcode && res.errcode == 'already_exist')
            {
                // setMsg
                setErrMsg("Page already exist.");
                setSev("error");
                // setOpen
                setMsgBarOpen(true);
            }
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));

    // close dlg
    setNewPageDlgOpen(false);
  };

  // Delete Page dialog
  const [delPageDlgOpen, setDelPageDlgOpen] = React.useState(false);

  const handleCloseDelPageDlg = () => {
    setDelPageDlgOpen(false);
  };

  const handleDeletePage = () => {
    var url_del = window.location.pathname + "?op=del";
    console.log("url_del:" + url_del);

    fetch(url_del, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          // set data to notify tree update
          Globaldata.treeDataShouldUpdate = "true";
          Globaldata.treeDataUpdateReason = "deletepage";

          // get parent node id
          var keypath = Utils.getKeyPathOfNode(
            Globaldata.treeInstance.state.treeData,
            Globaldata.selectedTreeNode
          );
          var keylist = keypath.split("/");

          var parentNodeKey;
          if (keylist.length >= 2) parentNodeKey = keylist[keylist.length - 2];
          else parentNodeKey = "";

          Globaldata.nodeToRefresh = parentNodeKey;

          Globaldata.preferredSelecKey = parentNodeKey;

          // build url to view parent node (or space root)
          var url_view;
          if (parentNodeKey.length > 0) {
            var path = Utils.getNodePathByNodeId(
              Globaldata.treeInstance.state.treeData,
              parentNodeKey
            );
            url_view =
              Utils.getSpacePathFromUrl(window.location.pathname) +
              "/pages" +
              path +
              "?op=view";
          } else {
            url_view =
              Utils.getSpacePathFromUrl(window.location.pathname) + "/pages";
          }

          history.push(url_view);
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));

    setDelPageDlgOpen(false);
  };

  // Export page dlg

  const [exportPageDlgOpen, setExportPageDlgOpen] = React.useState(false);
  const [exportFormat, setExportFormat] = React.useState("HTML");

  const handleCloseExportPageDlg = () => {
    setExportPageDlgOpen(false);
  };

  const handleExportFormatChange = (event) => {
    setExportFormat(event.target.value);
  };

  const handleExportPage = () => {
    // close dlg
    setExportPageDlgOpen(false);

    var pagetitle = Globaldata.titleOfCurDisplayedPage;

    if (!pagetitle) return;

    var url_export = window.location.pathname + "?op=export?fmt=html";

    fetch(url_export, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res) {
            console.dir(res);
            var utf8bytes = new TextEncoder().encode(res.data);
            Utils.saveFile(utf8bytes, pagetitle + ".html");
          }
        } else {
          console.log("Unexpected error");
        }
      })
      .catch((error) => console.log("error is", error));
  };

  const handleCloseMsgBar = () => {
    setMsgBarOpen(false);
  };

  // change password dialog
  const [changePwdDlgOpen, setChangePwdDlgOpen] = React.useState(false);

  const handleCloseChangePwdPageDlg = () => {
    setChangePwdDlgOpen(false);
  };

  const [oldpwd, setOldpwd] = React.useState("");
  const [newpwd, setNewpwd] = React.useState("");
  const [confirmpwd, setConfirmpwd] = React.useState("");

  const handleOldpwdChange = (event) => {
    setOldpwd(event.target.value);
  };
  const handleNewpwdChange = (event) => {
    setNewpwd(event.target.value);
  };
  const handleConfirmpwdChange = (event) => {
    setConfirmpwd(event.target.value);
  };

  const handleChangePwd = () => {
    if (oldpwd.length === 0 || newpwd.length === 0 || confirmpwd.length === 0)
      return;

    if (newpwd !== confirmpwd) {
      setChpwd_dlg_errmsg("Password inconsist.");
    }

    var hexstr_sha256 = Utils.SHA256(newpwd);
    var uint8_arr = Utils.fromHex(hexstr_sha256);
    var credential = window.btoa(
      String.fromCharCode(...new Uint8Array(uint8_arr))
    );

    var hexstr_sha256_old = Utils.SHA256(oldpwd);
    var uint8_arr_old = Utils.fromHex(hexstr_sha256_old);
    var oldcredential = window.btoa(
      String.fromCharCode(...new Uint8Array(uint8_arr_old))
    );

    var postdata = { sid: Utils.getSessionId(), oldcredential, credential };

    var url_update_credential = "/admin/users?op=updatecredential";

    fetch(url_update_credential, {
      method: "post",
      body: JSON.stringify(postdata),
    })
      .then((response) => response.text())
      .then((data) => {
        setChangePwdDlgOpen(false);

        var res = JSON.parse(data);
        if (res.status === "ok") {
          setErrMsg("Password updated.");
          setSev("success");
          setMsgBarOpen(true);
        } else {
          // setMsg
          setErrMsg("Failed to change password.");
          setSev("error");
          // setOpen
          setMsgBarOpen(true);
        }
      })
      .catch((error) => console.log("error is", error));

    /*
    });
        
    });
    */
  };

  const handleSearchInputChange = (event) => {
    searchInput = event.target.value;
    GlobalVarMgr.setSearchInput(searchInput);
  };

  const handleSearchIconClicked = () => {
    //console.log("search input:"+searchInput);
    if (searchInput.length > 0) {
      GlobalVarMgr.setSearchInput(searchInput);
      history.push("/search");
    }
  };

  const onKeyPress = (e) => {
    if (e.key === "Enter") {
      handleSearchIconClicked();
      e.preventDefault();
    }
  };

  const onSplitterMouseUp = (event) => {
    document.removeEventListener("mousemove", onSplitterMouseMove);
    document.removeEventListener("mouseup", onSplitterMouseUp);

    currentDrawerWidth = event.clientX;

    setDrawerWidth(event.clientX);
  };

  const onSplitterMouseMove = (event) => {
    setSplitterPos(event.clientX);
  };

  const onSplitterMouseDown = (event) => {
    //console.log("x="+event.clientX);

    // Only handle left clicks
    if (event.button !== 0) {
      return;
    }

    // Avoid text selection
    event.preventDefault();

    document.addEventListener("mouseup", onSplitterMouseUp);
    document.addEventListener("mousemove", onSplitterMouseMove);
  };

  // disable context menu on Treeview
  const onTreeviewContextMenu = (event) => {
    event.preventDefault();
    document.removeEventListener("contextmenu", onTreeviewContextMenu);
    
    setContextMenuX(event.clientX);
    setContextMenuY(event.clientY);
    setTreeContextMenuOpen(true);
  };

  const onTreeviewMouseDown = (event) => {
    if (event.button === 2) {
      //event.preventDefault();
      document.addEventListener("contextmenu", onTreeviewContextMenu);
    }
  };

  const handlePageLocationChange = (event) => {
    setPageLocation(event.target.value);
  };

  //about dialog
  const [aboutDlgOpen, setAboutDlgOpen] = React.useState(false);
  const [sysVer, setSysVer] = React.useState("");

  const handleCloseAboutDlg = () => {
    setAboutDlgOpen(false);
  };

  const handleAboutMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    var url_about = "/admin?op=sysver";

    fetch(url_about, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          //history.push('/admin');
          if (res.sysver) setSysVer(res.sysver);

          setAboutDlgOpen(true);
        } else {
        }
      })
      .catch((error) => console.log("error is", error));
  };

  //language dialog
  var curLang;
  if (GlobalVarMgr.getLang() === "zh_cn") curLang = "简体中文";
  else curLang = "English";

  const [langDlgOpen, setLangDlgOpen] = React.useState(false);
  const [langValue, setLangValue] = React.useState(curLang);

  const handleLangChange = (event) => {
    setLangValue(event.target.value);
  };

  const handleCloseLangDlg = () => {
    setLangDlgOpen(false);
  };

  const handleLangDlgOk = () => {
    if (langValue === "简体中文") GlobalVarMgr.setLang("zh_cn");
    else GlobalVarMgr.setLang("en");

    setLangDlgOpen(false);
  };

  const handleLangMenuItemClicked = () => {
    // close menu
    setAnchorEl(null);

    setLangDlgOpen(true);
  };
  
  //drawing
  const [drawingContainerOpen, setDrawingContainerOpen] = React.useState(false);
  
  const handleCancelDrawingContainer = () => {
      setDrawingContainerOpen(false);
  }

  const handleCloseDrawingContainer = () => {
    
    var wnd=document.getElementsByTagName("iframe");
    
    var found = false;
    var __wnd;
    
    for (var i in wnd) {
        
        var _wnd = wnd[i];
        
        if (_wnd.contentWindow && _wnd.contentWindow.svgEditor)
        {
            __wnd = _wnd;
            found = true;
            break;
        }
    }
    
    if (!found)
        return;
                
    var svg=__wnd.contentWindow.svgEditor.svgCanvas.svgCanvasToString();
    
    Globaldata.fnDrawingDone(svg);
    
    setDrawingContainerOpen(false);
  };
  
  Globaldata.fnEnableDrawing = setDrawingContainerOpen;

  function RenderSearchBox() {
    //if (!Utils.windowIsNarrow())
    if (1) {
      return (
        <>
          <OutlinedInput
            sx={{
               "& .MuiInputBase-input": {
                 pt: "4px", pb: "4px"
               },
            }}
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
        </>
      );
    } else {
      return <></>;
    }
  }
  
  function DrawingContainer(props) {

      return (
        <iframe src="/tinymce/js/tinymce/plugins/draw/editor/index.html" height={props.height}> </iframe>
      );
  }

  // check if op=edit
  var editting = false;
  var queryobj = Utils.parse(window.location.href);
  if (queryobj.op && queryobj.op === "edit") editting = true;

  return (
    <Box sx={{ display: "flex" }}>
      <CssBaseline />
      <AppBar variant="dense"
        position="absolute"
        elevation={1}
        open={open}
        ref={(node) => {
          if (node && node.clientHeight)
            Globaldata.appBarHeight = node.clientHeight;
        }}
      >
        <Toolbar id="ref_bar" variant="dense"
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
            }}
          >
            <MenuIcon />
          </IconButton>

          <RenderProdName />

          <RenderSearchBox />

          {/*
          <IconButton color="inherit">
            <Badge badgeContent={4} color="secondary">
              <NotificationsIcon />
            </Badge>
          </IconButton>
          */}

          <IconButton
            aria-controls="menu-appbarSignnin"
            aria-haspopup="true"
            onClick={handleCreateNewPageMenuClicked}
            edge="end"
            color="inherit"
          >
            <AddIcon />
          </IconButton>

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

          <IconButton
            aria-label="display more actions"
            aria-controls="menu-appbar"
            aria-haspopup="true"
            onClick={handleMenu}
            edge="end"
            color="inherit"
          >
            <MoreIcon />
          </IconButton>

          <Menu
            id="menu-appbar"
            anchorEl={anchorEl}
            anchorOrigin={{
              vertical: "top",
              horizontal: "right",
            }}
            keepMounted
            transformOrigin={{
              vertical: "top",
              horizontal: "right",
            }}
            open={menuopen}
            onClose={handleMenuClose}
          >
            <MenuItem onClick={handleCreateNewPageMenuClicked}>
              {Lang.str_menu_createpage}
            </MenuItem>
            {/*
            <MenuItem onClick={handleMovePageUpItemClicked}>{Lang.str_menu_move_page_up}</MenuItem>
            <MenuItem onClick={handleMovePageDownItemClicked}>{Lang.str_menu_move_page_down}</MenuItem>
            */}
            <MenuItem onClick={handleDelPageMenuItemClicked}>
              {Lang.str_menu_del_page}
            </MenuItem>
            {/*<MenuItem onClick={handleRenamePageMenuItemClicked}>{Lang.str_menu_rename}</MenuItem>*/}
            <MenuItem onClick={handlePageHistoryMenuItemClicked}>
              {Lang.str_menu_page_history}
            </MenuItem>
            <MenuItem onClick={handleAttachmentMenuItemClicked}>
              {Lang.str_menu_attachment}
            </MenuItem>
            <MenuItem onClick={handleExportPageMenuItemClicked}>
              {Lang.str_menu_export}
            </MenuItem>
            <MenuItem onClick={handleChangeSpaceMenuItemClicked}>
              {Lang.str_menu_space}
            </MenuItem>
            <MenuItem onClick={handleAdminMenuItemClicked}>
              {Lang.str_menu_admin_panel}
            </MenuItem>
            <MenuItem onClick={handleLangMenuItemClicked}>
              {Lang.str_menu_language}
            </MenuItem>
            <MenuItem onClick={handleAboutMenuItemClicked}>
              {Lang.str_dlg_about_title}
            </MenuItem>
            {/*<MenuItem onClick={handleMenuClose}>Export PDF</MenuItem>*/}
          </Menu>

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
            <ChangeUserPwdMenuItem />

            <MenuItem onClick={handleSignInMenuItemClicked}>
              <SignInMenuText />
            </MenuItem>
          </Menu>

          <Menu
            id="menu-treeContext"
            anchorReference="anchorPosition"
            anchorPosition={{ top: contextMenuY, left: contextMenuX }}
            anchorOrigin={{
              vertical: "top",
              horizontal: "left",
            }}
            keepMounted
            transformOrigin={{
              vertical: "top",
              horizontal: "left",
            }}
            open={treeContextMenuOpen}
            onClose={handleTreeContextMenuClose}
          >

            <MenuItem onClick={handleNewPageContextMenuItemClicked}>
            {Lang.str_ctxmenu_new}
            </MenuItem>
            <MenuItem onClick={handleNewChildPageContextMenuItemClicked}>
            {Lang.str_ctxmenu_new_child}
            </MenuItem>
          </Menu>

          <Dialog
            open={newPageDlgOpen}
            onClose={handleCloseNewPageDlg}
            aria-labelledby="form-dialog-title"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="form-dialog-title">
              {Lang.str_dlg_createpage}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                {/*<DialogContentText>
                {Lang.str_dlg_input_title}
              </DialogContentText>*/}
                <TextField
                  autoFocus
                  margin="dense"
                  id="name"
                  label={Lang.str_dlg_pagetitle}
                  onChange={handlePageTitleInputChange}
                  fullWidth
                />
                
                {/*
                <Select
                  displayEmpty
                  inputProps={{ "aria-label": "Without label" }}
                >
                  <MenuItem value={'Default Width'}>
                    {'Default Width'}
                  </MenuItem>
                  <MenuItem value={'PC Screen Width'}>
                    {'PC Screen Width'}
                  </MenuItem>
                </Select>*/}

                {typeof Globaldata.selectedTreeNode === "string" &&
                  Globaldata.selectedTreeNode.length > 0 && (
                    <>
                      <p />

                      <Select
                        value={pageLocation}
                        displayEmpty
                        inputProps={{ "aria-label": "Without label" }}
                        onChange={handlePageLocationChange}
                      >
                        <MenuItem value={Lang.str_createpagedlg_putpageafter}>
                          {Lang.str_createpagedlg_putpageafter}
                        </MenuItem>
                        <MenuItem value={Lang.str_createpagedlg_putpagebefore}>
                          {Lang.str_createpagedlg_putpagebefore}
                        </MenuItem>
                        <MenuItem value={Lang.str_createpagedlg_putpagechild}>
                          {Lang.str_createpagedlg_putpagechild}
                        </MenuItem>
                        <MenuItem value={Lang.str_createpagedlg_putpageroot}>
                          {Lang.str_createpagedlg_putpageroot}
                        </MenuItem>
                      </Select>
                    </>
                  )}
              </Paper>
              
              <Typography variant="caption" style={{ color: "#ff0000" }}>
                {newpgDlgErrmsg}
              </Typography>
          
            </DialogContent>
            <DialogActions>
              <Button onClick={handleCreateNewPage} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
              <Button onClick={handleCloseNewPageDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={delPageDlgOpen}
            onClose={handleCloseDelPageDlg}
            aria-labelledby="delpage-dialog-title"
          >
            <DialogTitle id="delpage-dialog-title">
              {Lang.str_dlg_delpage}
            </DialogTitle>
            <DialogContent>
              <DialogContentText>
                <DelPageDlgText />
              </DialogContentText>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleDeletePage} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
              <Button onClick={handleCloseDelPageDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={exportPageDlgOpen}
            onClose={handleCloseExportPageDlg}
            aria-labelledby="exportpage-dialog-title"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="exportpage-dialog-title">
              {Lang.str_dlg_exportpage}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                <Select
                  size="small"
                  value={exportFormat}
                  displayEmpty
                  inputProps={{ "aria-label": "Without label" }}
                  onChange={handleExportFormatChange}
                >
                  <MenuItem value={"HTML"}>HTML</MenuItem>
                </Select>
                
                <p> </p>
                <Typography variant="caption" style={{ color: "#ff0000" }}>
                 {"This is a preview feature. Development isn't completed yet."}
                </Typography>

              </Paper>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleExportPage} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
              <Button onClick={handleCloseExportPageDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={changePwdDlgOpen}
            onClose={handleCloseChangePwdPageDlg}
            aria-labelledby="changepwd-dialog-title"
          >
            <DialogTitle id="changepwd-dialog-title">
              Change Password
            </DialogTitle>
            <DialogContent>
              <TextField
                autoFocus
                margin="dense"
                id="oldpwd"
                type="password"
                label="Old Password"
                onChange={handleOldpwdChange}
                fullWidth
              />
              <TextField
                margin="dense"
                id="newpwd"
                type="password"
                label="New Password"
                onChange={handleNewpwdChange}
                fullWidth
              />
              <TextField
                margin="dense"
                id="chpwd-confirmpwd"
                type="password"
                label="Confirm Password"
                onChange={handleConfirmpwdChange}
                fullWidth
              />

              <Typography variant="caption" style={{ color: "#ff0000" }}>
                {chpwd_dlg_errmsg}
              </Typography>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleChangePwd} color="primary">
                Ok
              </Button>
              <Button onClick={handleCloseChangePwdPageDlg} color="primary">
                Cancel
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={renamePageDlgOpen}
            onClose={handleCloseRenamePageDlg}
            aria-labelledby="form-dialog-title"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="form-dialog-title">
              {Lang.str_menu_rename}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                <TextField
                  autoFocus
                  margin="dense"
                  id="rename_title"
                  label={Lang.str_dlg_pagetitle}
                  onChange={handleRenameTitleInputChange}
                  fullWidth
                />
              </Paper>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleRenamePage} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
              <Button onClick={handleCloseRenamePageDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={changeSpaceDlgOpen}
            onClose={handleCloseChangeSpaceDlg}
            aria-labelledby="form-dialog-title"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="form-dialog-title">
              {Lang.str_dlg_change_space}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                {userReadableSpaces.map((item) => (
                  <Link
                    key={item.spacename}
                    href={item.path + "/pages"}
                    underline="none"
                  >
                    {item.spacename}
                  </Link>
                ))}
              </Paper>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleCloseChangeSpaceDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={langDlgOpen}
            onClose={handleCloseLangDlg}
            aria-labelledby="form-dialog-title-lang"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="form-dialog-title-lang">
              {Lang.str_menu_language}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                <Typography variant="caption">
                  {Lang.str_dlg_lang_refresh}
                </Typography>

                <FormControl>
                  <FormLabel id="language-radio-buttons-group"></FormLabel>
                  <RadioGroup
                    aria-labelledby="language-radio-buttons-group"
                    name="lang-radio-buttons-group"
                    value={langValue}
                    onChange={handleLangChange}
                  >
                    <FormControlLabel
                      value="English"
                      control={<Radio />}
                      label="English"
                    />
                    <FormControlLabel
                      value="简体中文"
                      control={<Radio />}
                      label="简体中文"
                    />
                  </RadioGroup>
                </FormControl>
              </Paper>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleLangDlgOk} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
              <Button onClick={handleCloseLangDlg} color="primary">
                {Lang.str_dlg_btn_cancel}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            open={aboutDlgOpen}
            onClose={handleCloseAboutDlg}
            aria-labelledby="form-dialog-title-about"
            fullWidth={true}
            maxWidth={"sm"}
          >
            <DialogTitle id="form-dialog-title-about">
              {Lang.str_dlg_about_title}
            </DialogTitle>
            <DialogContent>
              <Paper
                elevation={0}
                sx={{
                  display: "flex",
                  flexDirection: "column",
                }}
              >
                <Typography variant="body2">
                  <Link href="https://github.com/lisupeng/tsailun">{sysVer}</Link>
                </Typography>
              </Paper>
            </DialogContent>
            <DialogActions>
              <Button onClick={handleCloseAboutDlg} color="primary">
                {Lang.str_dlg_btn_ok}
              </Button>
            </DialogActions>
          </Dialog>
          
          <Dialog
            fullScreen
            open={drawingContainerOpen}
            TransitionComponent={Transition}
          >
          
            <AppBar sx={{ position: 'relative' }} elevation={1} variant="dense">
              <Toolbar variant="dense">
                <Typography sx={{ ml: 2, flex: 1 }} variant="h6" component="div">
                    {" "}
                </Typography>
                
                <Button color="inherit" onClick={handleCancelDrawingContainer}>
                  Cancel
                </Button>
                
                <Button color="inherit" onClick={handleCloseDrawingContainer}>
                  Done
                </Button>
                
                {/*<IconButton
                  edge="start"
                  color="inherit"
                  aria-label="close"
                  onClick={handleCloseDrawingContainer}
                >
                <CloseIcon />
                </IconButton>*/}
              </Toolbar>
            </AppBar>
            
            <DrawingContainer height={window.innerHeight - Globaldata.appBarHeight} />

          </Dialog>
        </Toolbar>
      </AppBar>
      <Drawer
        id="pg_treeview"
        variant="permanent"
        open={open}
        onMouseDown={onTreeviewMouseDown}
      >

        <Paper
          id="pg_treeview_paper"
          elevation={0}
          sx={{
            p: 0,
            mt: `calc(${Globaldata.appBarHeight}px)`,
            display: "flex",
            flexDirection: "column",
            height: `calc(100vh - ${Globaldata.appBarHeight}px)`,
            overflow: "auto",
          }}
        >
          <Box sx={{ p: 0, mt: "35px", }}>
            <CAntdtree />
          </Box>
        </Paper>
      </Drawer>
      {open && editting === false && (
        <Box
          position="absolute"
          onMouseDown={onSplitterMouseDown}
          sx={{
            display: "flex",
            left: splitterPos - 3,
            top: Globaldata.appBarHeight,
            width: 5,
            zIndex: 1250, // drawer:1200, modal:1300, snackbar: 1400, tooltip:1500
            height: `calc(100vh - ${Globaldata.appBarHeight}px)`,
            backgroundColor: "#e0e0e0",
            opacity: [0, 0, 0],
            "&:hover": {
              cursor: "w-resize",
              backgroundColor: "#e0e0e0",
              opacity: [0.9, 0.8, 0.7],
            },
          }}
        ></Box>
      )}

      <Box
        id="right_widget"
        component="main"
        sx={{
          /*
          backgroundColor: (theme) =>
            theme.palette.mode === 'light'
              ? theme.palette.grey[100]
              : theme.palette.grey[900],*/
          backgroundColor: "#ffffff",/*(theme) => 
            theme.palette.mode === "light" && !Utils.windowIsNarrow()
              ? theme.palette.grey[100]
              : "#ffffff",*/
          flexGrow: 1,
          height: `calc(100vh - ${Globaldata.appBarHeight}px)`,
          mt: `calc(${Globaldata.appBarHeight}px)`,
          overflow: "auto",
        }}
      >

        <Container
          id="tsl_container"
          maxWidth="lg"
          disableGutters={editting === true}
          sx={{
            mt: 0,
            mb: 0,
            ml: (!Utils.windowIsNarrow() && !editting)? 3:0,
            //mr: 0,
            backgroundColor: "#ffffff",
            //border: 1,
            //borderRight: 1,
            //borderColor: "#f0f0f0",
            minHeight: `calc(100vh - ${Globaldata.appBarHeight}px)`,
          }}
        >
          {" "}
          {/* top/bottom margin */}
          <Snackbar
            open={msgBarOpen}
            anchorOrigin={{ vertical: "top", horizontal: "center" }}
            autoHideDuration={3000}
            onClose={handleCloseMsgBar}
          >
            <Alert onClose={handleCloseMsgBar} severity={sev}>
              {" "}
              {errmsg}{" "}
            </Alert>
          </Snackbar>
          <Contentarea />
        </Container>
      </Box>
    </Box>
  );
}

export default function MainView() {
  return <MainViewContent />;
}
