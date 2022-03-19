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

import axios from 'axios';

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

function axiowrapper (url, data) {

    return new Promise((resolve, reject) => {
        axios.post(url, data).then((res) => {
          resolve(res);
        }).catch((err) => {
          reject(err);
        })
      })
}

function readfilewrapper (file) {

 return new Promise(function (resolve, reject) {

  let reader = new FileReader();

  reader.onload = function () {
   resolve(reader);
  };

  reader.onerror = reject;

  reader.readAsDataURL(file);
 });
}

async function upload(file, successCallback, failureCallback, progressCallback) {
    
    const chunk_size = 1 * 1024 * 1024;
    
    //const chunk_size = 30 * 1024;
    
    var seq = 0;
    var url = '';
    
    var done = false;
    
    var fileurl = '';
    
    var id = Utils.genUUID();

    while (done === false)
    {
        var begin = seq*chunk_size;
        var end = (seq+1)*chunk_size;
        
        if (begin >= file.size)
        {
            done = true;
            break;
        }
        
        var blob = file.slice(begin, end);
        let read_ret = await readfilewrapper(blob);
        
        var _chunkdata = read_ret.result
        
        if (!(_chunkdata))
        {
            failureCallback('Upload failed');
            return;
        }
        
        var ibegin = _chunkdata.indexOf(";base64,");
        if (ibegin === -1) return;

        ibegin = ibegin + ";base64,".length;
        var chunkdata = _chunkdata.substring(ibegin);
        
        var postdata = JSON.stringify({ sid: Utils.getSessionId(), chunkdata })
    
        url = window.location.pathname + "?op=uploadchunk&filename=" + encodeURIComponent(file.name)+"&seq="+seq+"&chunksize="+chunk_size+"&tid="+id;
        let ret = await axiowrapper(url, postdata);
        
        fileurl = ret.data.url;
        
        seq++;
        
        const progress = (seq*chunk_size / file.size * 100 | 0) + '%';
        progressCallback(progress);
    }
    
    successCallback(fileurl);
}

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

const content_style = "body { font-family:Helvetica,Arial,sans-serif; font-size:16px } p{ margin:2px 0} " + 
                      ".attachment {cursor: pointer !important; } " + 
                      ".upload_error {background: #FFE5E0; border: 1px solid #EA644A; } " + 
                      ".attachment > img {     width: 32px;    vertical-align: middle;    padding-right:4px;} " + 
                      ".attachment > a {    text-decoration: none;    vertical-align: middle;} " + 
                      ".attachment > span {    vertical-align: middle;    padding-right:4px;}";

export default function Editorwrap() {
  let history = useHistory();
  const [gotcontent, setGotcontent] = React.useState(false);
  const [exitPromp, setExitPromp] = React.useState(true);
  //const [forceRedrawCnt, setForceRedrawCnt] = React.useState(0);
  const [saving, setSaving] = React.useState(false);

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

            document.title = 'Edit - '+res.title;
            
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

      if (saving === false) {
        setSaving(true);
      }

      // save page data and redirect to view page
      fetch(url_save, {
        method: "post",
        body: JSON.stringify({ sid: Utils.getSessionId(), content: content }),
      })
        .then((response) => response.text())
        .then((data) => {
          setSaving(false);

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
        .catch((error) => {
          console.log("error is", error);
          setSaving(false);
        });
    }
  };

  editorHeight = window.innerHeight - Globaldata.appBarHeight;

  window.onresize = function () {
    editorHeight = window.innerHeight - Globaldata.appBarHeight;
    
    // resize editor
    if (editorRef.current && editorRef.current.editorContainer)
    {
        editorRef.current.editorContainer.style.height = ""+editorHeight+"px";
    }

  };

  if (gotcontent)
    return (
      <>
        {saving === false && (
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
                    "searchreplace visualblocks fullscreen",
                    "insertdatetime media table powerpaste code help wordcount attachment toc", // DON'T use paste plugin, otherwise unable paste images.
                  ],
                  toolbar: [
                    "undo redo | bold italic underline strikethrough forecolor backcolor | fontselect | fontsizeselect | formatselect | toc | bullist numlist | lineheight outdent indent alignleft aligncenter alignright alignjustify | table image link anchor | removeformat attachment",
                  ],
                  content_style: content_style,
                  relative_urls: false,
                  automatic_uploads: false,
                  images_replace_blob_uris: false,
                  
                  // Limit to 1T
                  attachment_max_size: 1000 * 1000 * 1024 * 1024,
                  attachment_assets_path: '/misc/assets/icons/',
                  attachment_upload_handler: upload,
                  toc_depth: '6',
                  
                  /*block_unsupported_drop: false*/
                  
                  /*,
               paste_data_images: true,
               images_upload_handler: example_image_upload_handler
               */
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
        )}
      </>
    );
  else return <div></div>;
}
