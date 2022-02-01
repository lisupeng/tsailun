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
import { useHistory } from "react-router-dom";
import { Editor } from "@tinymce/tinymce-react";
import Utils from "../Utils";
import Globaldata from "../Globaldata";

import Paper from "@material-ui/core/Paper";

import Fab from "@material-ui/core/Fab";
import DoneIcon from "@material-ui/icons/Done";

import { Prompt } from "react-router-dom";

import Lang from "../../i18n/Lang";

var initContent = "";

var editorHeight = 700;

const buttonStyle = {
  margin: 0,
  top: "auto",
  right: 30,
  bottom: 40,
  left: "auto",
  position: "fixed",
};

function example_image_upload_handler(blobInfo, success, failure, progress) {
  console.log("example_image_upload_handler()");
  console.dir(blobInfo);

  console.log("filename:" + blobInfo.filename());
  /*
  var xhr, formData;

  xhr = new XMLHttpRequest();
  xhr.withCredentials = false;
  xhr.open('POST', 'postAcceptor.php');

  xhr.upload.onprogress = function (e) {
    progress(e.loaded / e.total * 100);
  };

  xhr.onload = function() {
    var json;

    if (xhr.status === 403) {
      failure('HTTP Error: ' + xhr.status, { remove: true });
      return;
    }

    if (xhr.status < 200 || xhr.status >= 300) {
      failure('HTTP Error: ' + xhr.status);
      return;
    }

    json = JSON.parse(xhr.responseText);

    if (!json || typeof json.location != 'string') {
      failure('Invalid JSON: ' + xhr.responseText);
      return;
    }

    success(json.location);
  };

  xhr.onerror = function () {
    failure('Image upload failed due to a XHR Transport error. Code: ' + xhr.status);
  };

  formData = new FormData();
  formData.append('file', blobInfo.blob(), blobInfo.filename());

  xhr.send(formData);
  */
}

export default function Editorwrap() {
  let history = useHistory();
  const [gotcontent, setGotcontent] = React.useState(false);
  const [exitPromp, setExitPromp] = React.useState(true);
  const [forceRedrawCnt, setForceRedrawCnt] = React.useState(0);

  //var initContent = "";

  // DOM did mount - BEGIN
  React.useEffect(() => {
    var page_full_path = window.location.pathname;
    var url_readpage = page_full_path + "?op=read";
    var queryobj = Utils.parse(window.location.href);
    if (queryobj.ver) url_readpage = url_readpage + "&ver=" + queryobj.ver;
    //console.log("read page for editing, url:"+url_readpage);

    // get page data
    fetch(url_readpage, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          if (res.body) {
            //console.log("body="+res.body);
            initContent = res.body;
            setGotcontent(true);
          }
        } else {
          if (res.errcode === "invalid_session") {
            // redirect to signin
            Utils.clearSession();
            history.push("/signin");
          } else {
            console.log("Unexpected error");
          }
        }
      })
      .catch((error) => console.log("error is", error));
  }, []);
  // DOM did mount - END

  const editorRef = useRef(null);
  const onSave = () => {
    if (editorRef.current) {
      var content = "<body>" + editorRef.current.getContent() + "</body>";
      var page_full_path = window.location.pathname;
      var url_save = page_full_path + "?op=save";

      // save page data and redirect to view page
      fetch(url_save, {
        method: "post",
        body: JSON.stringify({ sid: Utils.getSessionId(), content: content }),
      })
        .then((response) => response.text())
        .then((data) => {
          var res = JSON.parse(data);
          if (res.status === "ok") {
            setExitPromp(false);

            // redirect to view page
            var url_view = page_full_path + "?op=view";
            history.push(url_view);
          } else {
            console.log("Unexpected error");
          }
        })
        .catch((error) => console.log("error is", error));
    }
  };

  editorHeight = window.innerHeight - Globaldata.appBarHeight;

  window.onresize = function () {
    // force redraw to resize editor
    setForceRedrawCnt(forceRedrawCnt + 1);
  };

  if (gotcontent)
    return (
      <>
        <div>
          <Prompt
            when={exitPromp}
            message={() => Lang.str_editor_leave_confirm}
          />
        </div>

        <Paper
          sx={{
            p: 0,
            display: "flex",
            flexDirection: "column",
            height: `calc(100vh - ${Globaldata.appBarHeight}px)`,
          }}
        >
          <Editor
            onInit={(evt, editor) => (editorRef.current = editor)}
            initialValue={initContent}
            init={{
              height: editorHeight,
              menubar: false,
              statusbar: false,

              plugins: [
                "advlist autolink lists link image charmap print preview anchor",
                "searchreplace visualblocks code fullscreen",
                "insertdatetime media table help wordcount",
              ],
              toolbar: [
                "undo redo | bold italic underline strikethrough forecolor backcolor | fontselect | fontsizeselect | formatselect| bullist numlist | lineheight outdent indent alignleft aligncenter alignright alignjustify | table image link anchor | removeformat",
              ],
              content_style:
                "body { font-family:Helvetica,Arial,sans-serif; font-size:16px }",
              relative_urls: false,
              automatic_uploads: false,
              images_replace_blob_uris: false,
            }}
          />

          <Fab
            color="primary"
            size="small"
            aria-label="save"
            onClick={onSave}
            style={buttonStyle}
          >
            <DoneIcon />
          </Fab>
        </Paper>
        {/*<button onClick={onSave}>Save</button>*/}
      </>
    );
  else return <div></div>;
}
