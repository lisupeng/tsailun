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

//import logo from './logo.svg';
//import './App.css';
import { BrowserRouter as Router, Route, Redirect, Switch} from 'react-router-dom';
import MainView from './components/ui/MainView'
import SignIn from './components/SignIn'
import SignUp from './components/SignUp'
import AdminDashboard from './components/ui/AdminDashboard'
import SearchView from './components/ui/SearchView'
import Livedemo from './components/Livedemo'

function App() {
  return (

    <Router>
    <Switch >

        <Route path="/spaces" component={MainView} />
        <Route path="/admin" component={AdminDashboard} />
        <Route path="/search" component={SearchView} />
        <Route path="/signin" component={SignIn} />
        <Route path="/signup" component={SignUp} />
        <Route path="/demo" component={Livedemo} />
        <Redirect exact from="/" to="/spaces/default/pages" />
    </Switch>
    </Router>

  );
}

export default App;
