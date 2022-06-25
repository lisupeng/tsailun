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
//import { useHistory } from 'react-router-dom';
//import PropTypes from 'prop-types';
import { withRouter } from "react-router-dom";
import { Tree } from "antd";
import "antd/dist/antd.css";
import "./workaround.css";
import Globaldata from "../Globaldata";
import Utils from "../Utils";

const { TreeNode } = Tree;

function updateTreeData(list, key, children) {
  return list.map((node) => {
    if (node.key === key) {
      return { ...node, children };
    }

    if (node.children) {
      return {
        ...node,
        children: updateTreeData(node.children, key, children),
      };
    }

    return node;
  });
}

function getIdByDirnameFromNodeList(list, dir) {
    
    for (var i = 0; i < list.length; i++)
    {
        var item = list[i];
        
        if (item.pagedir == dir && item.key.length > 0)
            return item.key;
    }
    return "";
}

class CAntdtree extends React.Component {
  state = {
    treeData: [],
    selectedKeys: [],
    expandedKeys: [],
    loadedKeys: [],
  };

  fetchTreeRootLevelData() {
    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages?op=listpage";

    // The reason put session id to post:
    //    User may save url for future use. It's better not put it to url

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          var initTreeData = res.list;
          this.setState({
            treeData: initTreeData,
            expandedKeys: [],
            loadedKeys: [],
            selectedKeys: [],
          });
        } else {
          if (res.errcode === "invalid_session") {
            // TODO
          }
        }
      })
      .catch((error) => console.log("error is", error));
  }
  
  // this can be optimized by implementing an new handler in backend to reply multiple levels tree data
  // in a single reply  
  async fetchMultiLevelTreeDataByPath()
  {
    var url_space = Utils.getSpacePathFromUrl(window.location.pathname);
    var path = Utils.getPagePathFromUrl(window.location.pathname);
    
    if (!(path && path.length > 2)) {
        this.fetchTreeRootLevelData();
        return;
    }
    
    var pathlist = path.split("/");
    
    if (pathlist.length < 2)
    {
        this.fetchTreeRootLevelData();
        return;
    }
    
    // put to front. this item stands for root path
    pathlist.unshift("");
    
    // empty tree keys
    var tdata = [];
    var selected = [];
    var expanded = [];
    var loaded = [];
    Globaldata.selectedTreeNode = "";
    
    var curpath = "";
    
    var toFetchNodeId = "";
    
    for (var i = 0; i < pathlist.length; i++) {
        var p = pathlist[i];
        
        if (i > 0) // 0 is root level
            curpath = curpath+"/"+p;
        
        var url = Utils.getSpacePathFromUrl(window.location.pathname) + "/pages"+curpath+"?op=listpage";

        var postdata = JSON.stringify({ sid: Utils.getSessionId()});
        let ret = await Utils.axiowrapper(url, postdata);
        
        // if fail, set existing data to tree and return
        if (!(ret && ret.data && ret.data.status && ret.data.status == "ok"))
        {
            this.setState({
              treeData: tdata,
              expandedKeys: expanded,
              loadedKeys: loaded,
              selectedKeys: selected,
            });
            return;
        }
        
        if (i == 0) // root node
        {
            tdata = ret.data.list;
        }
        else
        {
          if (toFetchNodeId.length > 0)
          {
              tdata = updateTreeData(tdata, toFetchNodeId, ret.data.list);
              expanded.push(toFetchNodeId);
              Globaldata.selectedTreeNode = toFetchNodeId;
              loaded.push(toFetchNodeId);
          }
          else
          {
            this.setState({
              treeData: tdata,
              expandedKeys: expanded,
              loadedKeys: loaded,
              selectedKeys: selected,
            });
            return;
          }
        }
        
        // set next_p and next toFetchNodeId
        var next_p = "";
        if ((i+1) <= (pathlist.length - 1))
        {
            next_p = pathlist[i+1];
        }
        
        // getIdByDirnameFromNodeList
        toFetchNodeId = getIdByDirnameFromNodeList(ret.data.list, decodeURIComponent(next_p));
        
        if ((i+1) == (pathlist.length-1))
            selected.push(toFetchNodeId);
    }
    
    this.setState({
      treeData: tdata,
      expandedKeys: expanded,
      loadedKeys: loaded,
      selectedKeys: selected,
    });
  }

  fetchNodeChildData(nodeId, resolve) {
    var key = nodeId;

    var curNodePath = Utils.getNodePathByNodeId(this.state.treeData, key);
    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      curNodePath +
      "?op=listpage";

    fetch(url, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId() }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          var childNodeData = res.list;
          var origin = this.state.treeData;
          var newdata = updateTreeData(origin, key, childNodeData);

          this.setState({
            treeData: newdata,
          });

          var loadedKeys = this.state.loadedKeys;
          loadedKeys.push(nodeId);
          this.setState({ loadedKeys });

          if (resolve) resolve();
        } else {
          // TODO must handle this, tree data modified by other users. should refresh entire tree
        }
      })
      .catch((error) => console.log("error is", error));
  }

  refreshTreeData() {
    // reload entire tree
    this.fetchTreeRootLevelData();
  }

  // refresh node and it's children level data (children's children not included)
  refreshNodeData(nodekey, preferredSelecKey) {
    // if none node selected, refresh treeData
    if (!nodekey || nodekey === "") {
      this.refreshTreeData();
      return;
    }

    // get node object from tree
    var subtree = Utils.getNodeByKey(this.state.treeData, nodekey);
    if (!subtree) return;

    var affectedKeys = [];
    Utils.getKeyList(subtree.children, affectedKeys);

    // reloadNodeData
    var path = Utils.getNodePathByNodeId(this.state.treeData, nodekey);
    var url =
      Utils.getSpacePathFromUrl(window.location.pathname) +
      "/pages" +
      path +
      "?op=dirinfo";
    fetch(url)
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
          var newNodeData = res.node;

          // replace node with new one
          Utils.replaceNodeData(this.state.treeData, newNodeData);

          var expanded = Utils.arrayAminusB(
            this.state.expandedKeys,
            affectedKeys
          );
          var loaded = Utils.arrayAminusB(this.state.loadedKeys, affectedKeys);
          var selected = Utils.arrayAminusB(
            this.state.selectedKeys,
            affectedKeys
          );

          // set preferred selection if any. (make sure selected key in tree)
          if (preferredSelecKey && preferredSelecKey !== "") {
            selected = [];
            selected.push(preferredSelecKey);
          }

          this.setState({
            treeData: this.state.treeData,
            expandedKeys: expanded,
            loadedKeys: loaded,
            selectedKeys: selected,
          });
        }
      })
      .catch((error) => console.log("error is", error));
  }

  componentDidMount() {
    Globaldata.treeInstance = this;

    //this.fetchTreeRootLevelData();
    this.fetchMultiLevelTreeDataByPath();
  }

  onLoadData = (treeNode) => {
    return new Promise((resolve) => {
      if (treeNode.dataRef.children) {
        // Will hit here if children data already loaded in other place
        // Should not force reload only by removing children data here, should also update data like expandedKeys, selectedKeys ...
        resolve();
        return;
      }

      var key = treeNode.dataRef.key;
      this.fetchNodeChildData(key, resolve);
    });
  };

  onExpand = (expandedKeys) => {
    // if not set autoExpandParent to false, if children expanded, parent can not collapse.
    // or, you can remove all expanded children keys.
    this.setState({
      expandedKeys,
      //autoExpandParent: false,
    });
  };

  onSelect(selectedKeys, { selected, selectedNodes, node, event }) {
    this.setState({ selectedKeys });

    if (selected == true) {
      var nodeid = selectedKeys[0];
      var nodepath = Utils.getNodePathByNodeId(this.state.treeData, nodeid);

      Globaldata.selectedTreeNode = nodeid;

      Globaldata.titleOfSelectedNode = node.title;

      // redirect to view page
      var url_view =
        Utils.getSpacePathFromUrl(window.location.pathname) +
        "/pages" +
        nodepath +
        "?op=view";
      this.props.history.push(url_view);
    } else {
      // unselect
      Globaldata.selectedTreeNode = "";
      Globaldata.titleOfSelectedNode = "";
    }
  }

  renderTreeNodes = (data) =>
    data.map((item) => {
      if (item.children) {
        return (
          <TreeNode title={item.title} key={item.key} dataRef={item}>
            {this.renderTreeNodes(item.children)}
          </TreeNode>
        );
      }
      return <TreeNode key={item.key} {...item} dataRef={item} />;
    });

  render() {
    if (
      Globaldata.treeDataShouldUpdate === "true" &&
      Globaldata.treeDataUpdateReason === "createpage"
    ) {
      if (Globaldata.nodeToRefresh.length > 0) {
        this.refreshNodeData(
          Globaldata.nodeToRefresh,
          Globaldata.preferredSelecKey
        );
        Globaldata.nodeToRefresh = "";
      } else {
        this.refreshTreeData(); // refresh entire tree
      }

      Globaldata.treeDataShouldUpdate = "false";
      Globaldata.treeDataUpdateReason = "";
      Globaldata.preferredSelecKey = "";
    } else if (
      Globaldata.treeDataShouldUpdate === "true" &&
      Globaldata.treeDataUpdateReason === "deletepage"
    ) {
      if (Globaldata.nodeToRefresh.length > 0) {
        this.refreshNodeData(
          Globaldata.nodeToRefresh,
          Globaldata.preferredSelecKey
        );
        Globaldata.nodeToRefresh = "";
      } else {
        this.refreshTreeData(); // refresh entire tree
      }

      Globaldata.treeDataShouldUpdate = "false";
      Globaldata.treeDataUpdateReason = "";
      Globaldata.preferredSelecKey = "";
    } else if (
      Globaldata.treeDataShouldUpdate === "true" &&
      Globaldata.treeDataUpdateReason === "renamepage"
    ) {
      if (Globaldata.nodeToRefresh.length > 0) {
        this.refreshNodeData(
          Globaldata.nodeToRefresh,
          Globaldata.preferredSelecKey
        );
        Globaldata.nodeToRefresh = "";
      } else {
        this.refreshTreeData(); // refresh entire tree
      }

      Globaldata.treeDataShouldUpdate = "false";
      Globaldata.treeDataUpdateReason = "";
      Globaldata.preferredSelecKey = "";
    } else if (
      Globaldata.treeDataShouldUpdate === "true" &&
      (Globaldata.treeDataUpdateReason === "moveup" ||
        Globaldata.treeDataUpdateReason === "movedown")
    ) {
      this.refreshTreeData();

      Globaldata.treeDataShouldUpdate = "false";
      Globaldata.treeDataUpdateReason = "";
      Globaldata.preferredSelecKey = "";
    }

    return (
      <Tree
        loadData={this.onLoadData}
        loadedKeys={this.state.loadedKeys}
        onSelect={this.onSelect.bind(this)}
        selectedKeys={this.state.selectedKeys}
        onExpand={this.onExpand}
        expandedKeys={this.state.expandedKeys}
        ref={(node) => {
          /*console.dir(node)*/
        }}
      >
        {this.renderTreeNodes(this.state.treeData)}
      </Tree>
    );
  }
}

export default withRouter(CAntdtree);
