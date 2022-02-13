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

const GlobalVarMgr = {
  getLang() {
    if (!window.sessionStorage.lang)
      window.sessionStorage.lang = "en";

    return window.sessionStorage.lang;
  },

  setLang(val) {
    window.sessionStorage.lang = val;
  },
  
  getSearchInput() {
    if (!window.sessionStorage.searchInput)
      window.sessionStorage.searchInput = "";

    return window.sessionStorage.searchInput;
  },

  setSearchInput(val) {
    window.sessionStorage.searchInput = val;
  },
  
  getLastCheckupdateDay() {
    if (!window.sessionStorage.LastCheckupdateDay)
      window.sessionStorage.LastCheckupdateDay = -1;

    return window.sessionStorage.LastCheckupdateDay;
  },

  setLastCheckupdateDay(val) {
    window.sessionStorage.LastCheckupdateDay = val;
  },
};

export default GlobalVarMgr;
