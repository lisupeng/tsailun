!function(){"use strict";function i(t){t.focus(),t.getParam("enable_drawing_editor")(function(a){var n="<img alt='"+a+'\' class="tsailundraw" src="data:image/svg+xml;base64,'+window.btoa(unescape(encodeURIComponent(a)))+'" />';t.selection.setContent(n)},null)}tinymce.PluginManager.add("draw",function(a,n){var t,e;(t=a).ui.registry.addButton("draw",{icon:"image",tooltip:"Drawing",onAction:function(){return i(t)}}),t.ui.registry.addMenuItem("draw",{icon:"draw",text:"Drawing",onAction:function(){return i(t)}}),a.addCommand("mceDraw",function(){}),(e=a).on("DblClick",function(a){var n=null;a.target&&a.target.alt&&(n=a.target.alt),a.target.className&&"tsailundraw"==a.target.className&&e.getParam("enable_drawing_editor")(function(a){var n="<img alt='"+a+'\' class="tsailundraw" src="data:image/svg+xml;base64,'+window.btoa(unescape(encodeURIComponent(a)))+'" />';e.selection.setContent(n)},n)})})}();