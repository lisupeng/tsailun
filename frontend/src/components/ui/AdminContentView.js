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

import StatsView from "./StatsView";
import UsersView from "./UsersView";
import UsergroupsView from "./UsergroupsView";
import SpacesView from "./SpacesView";
import UserEditView from "./UserEditView";
import UserGroupEditView from "./UserGroupEditView";
import SpaceEditView from "./SpaceEditView";
import LogsView from "./LogsView";

export default function AdminContentView() {
  var url = window.location.pathname;

  if (url === "/admin/stats") return <StatsView />;
  else if (url === "/admin/users") return <UsersView />;
  else if (url === "/admin/usergroups") return <UsergroupsView />;
  else if (url === "/admin/spaces") return <SpacesView />;
  else if (url === "/admin/useredit") return <UserEditView />;
  else if (url === "/admin/usergroupedit") return <UserGroupEditView />;
  else if (url === "/admin/spaceedit") return <SpaceEditView />;
  else if (url === "/admin/logs") return <LogsView />;
  else return <> </>;
}
