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

import lang_zhcn from './zh_cn';
import lang_en from './en';

var Lang = {
    
    str_search_no_match: 'No match found',
    reserved: 0

}

var setting_lang = 'en';
//var setting_lang = '';

if (setting_lang === 'zh_cn')
    Lang = lang_zhcn;
else
    Lang = lang_en;

/*
(function() {
	Lang = lang_zhcn;

})();
*/

export default Lang;