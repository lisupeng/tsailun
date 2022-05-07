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
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import ListItemText from "@material-ui/core/ListItemText";
import ListSubheader from "@material-ui/core/ListSubheader";
import DashboardIcon from "@material-ui/icons/Dashboard";
//import ShoppingCartIcon from '@material-ui/icons/ShoppingCart';
//import RestoreFromTrashIcon from '@material-ui/icons/RestoreFromTrash';
import PeopleIcon from "@material-ui/icons/People";
import PersonIcon from "@material-ui/icons/Person";
import ExitToAppIcon from "@material-ui/icons/ExitToApp";
import BarChartIcon from "@material-ui/icons/BarChart";
//import LayersIcon from '@material-ui/icons/Layers';
import AssignmentIcon from "@material-ui/icons/Assignment";
//import SettingsIcon from '@material-ui/icons/Settings';

import List from "@material-ui/core/List";
import Divider from "@material-ui/core/Divider";

import Lang from "../../i18n/Lang";
//Settings

export default function AdminListItems() {
  let history = useHistory();

  const handleListItemClick = (event, index) => {
    if (index === "home") history.push("/");
    else history.push("/admin/" + index);
  };

  const mainListItems = (
    <div>
      <ListItem button onClick={(event) => handleListItemClick(event, "users")}>
        <ListItemIcon>
          <PersonIcon />
        </ListItemIcon>
        <ListItemText primary={Lang.str_admin_users} />
      </ListItem>

      <ListItem
        button
        onClick={(event) => handleListItemClick(event, "usergroups")}
      >
        <ListItemIcon>
          <PeopleIcon />
        </ListItemIcon>
        <ListItemText primary={Lang.str_admin_usergroups} />
      </ListItem>

      <ListItem
        button
        onClick={(event) => handleListItemClick(event, "spaces")}
      >
        <ListItemIcon>
          <DashboardIcon />
        </ListItemIcon>
        <ListItemText primary={Lang.str_admin_spaces} />
      </ListItem>
      {/*
        <ListItem button onClick={(event) => handleListItemClick(event, 'recyclebin')}>
          <ListItemIcon>
            <RestoreFromTrashIcon />
          </ListItemIcon>
          <ListItemText primary="Recycle Bin" />
        </ListItem>
    
        <ListItem button onClick={(event) => handleListItemClick(event, 'settings')}>
          <ListItemIcon>
            <SettingsIcon />
          </ListItemIcon>
          <ListItemText primary="Settings" />
        </ListItem>
        */}
    </div>
  );

  const secondaryListItems = (
    <div>
      <ListSubheader inset>{Lang.str_admin_sysreports}</ListSubheader>

      <ListItem button onClick={(event) => handleListItemClick(event, "stats")}>
        <ListItemIcon>
          <BarChartIcon />
        </ListItemIcon>
        <ListItemText primary={Lang.str_admin_stats} />
      </ListItem>

      <ListItem button onClick={(event) => handleListItemClick(event, "logs")}>
        <ListItemIcon>
          <AssignmentIcon />
        </ListItemIcon>
        <ListItemText primary={Lang.str_admin_systemlog} />
      </ListItem>
    </div>
  );

  return (
    <>
      <List>{mainListItems}</List>

        <Divider />
        <List>{secondaryListItems}</List>

      <Divider />
      <List>
        <div>
          <ListItem
            button
            onClick={(event) => handleListItemClick(event, "home")}
          >
            <ListItemIcon>
              <ExitToAppIcon />
            </ListItemIcon>
            <ListItemText primary={Lang.str_admin_exit} />
          </ListItem>
        </div>
      </List>
    </>
  );
}
