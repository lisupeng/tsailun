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

import React, { useRef } from "react";
import Typography from "@material-ui/core/Typography";
import Lang from "../../i18n/Lang";

export default function EmptySpacePrompt() {
  return (
    <>
      <Typography variant="body2" style={{ whiteSpace: "pre-line" }}>
        {"\n\n\n\n\n\n\n\n\n\n\n"}
      </Typography>

      <Typography variant="h5" style={{ color: "#c0c0c0" }} align="center">
        {Lang.str_pageviewer_no_page}
      </Typography>

      <Typography variant="body2" style={{ whiteSpace: "pre-line" }}>
        {"\n"}
      </Typography>

      <Typography variant="body2" style={{ color: "#c0c0c0" }} align="center">
        {Lang.str_pageviewer_create_first_page}
      </Typography>
    </>
  );
}
