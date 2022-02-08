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
import { Tree } from "antd";
import "antd/dist/antd.css";
import Globaldata from "../Globaldata";
import Utils from "../Utils";

var initTreeData = [
  {
    title: "Expand to load",
    key: "0",
  },
  {
    title: "Expand to load",
    key: "1",
  },
  {
    title: "Tree Node",
    key: "2",
    isLeaf: true,
  },
];

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

const fnInitData = (origin) => {
  return initTreeData;
};

export default function Antdtree() {
  const [treeData, setTreeData] = React.useState([]);
  const [selectedKeys, setSelectedKeys] = React.useState([]);

  let history = useHistory();

  function updateEntireTreeData() {
    fetch("/pages?op=listpage")
      .then((response) => response.text())
      .then((data) => {
        initTreeData = JSON.parse(data);
        setTreeData(fnInitData);
      })
      .catch((error) => console.log("error is", error));
  }

  React.useEffect(() => {
    updateEntireTreeData();
  }, []);

  function onLoadData({ key, children }) {
    console.log("onLoadData()");
    return new Promise((resolve) => {
      if (children) {
        resolve();
        return;
      }

      var curNodePath = Utils.getNodePathByNodeId(treeData, key);
      var url = "/pages" + curNodePath + "?op=listpage";

      fetch(url)
        .then((response) => response.text())
        .then((data) => {
          var childNodeData = JSON.parse(data);

          const fnUpdateData = (origin) => {
            return updateTreeData(origin, key, childNodeData);
          };

          setTreeData(fnUpdateData);
          resolve();
        })
        .catch((error) => console.log("error is", error));
    });
  }

  function onNodeSelected(
    selectedKeys,
    { selected, selectedNodes, node, event }
  ) {
    var curPath = "";

    setSelectedKeys(selectedKeys);

    if (selected == true) {
      var nodeid = selectedKeys[0];
      var path = Utils.getNodePathByNodeId(treeData, nodeid);

      Globaldata.selectedTreeNode = nodeid;

      // redirect to view page
      var url_view = "/pages" + path + "?op=view";
      history.push(url_view);
    } else {
      // unselect
      Globaldata.selectedTreeNode = "";
    }
  }

  function updateNodeData(nodeid, selectedKey) {
    var key = nodeid;
    if (!key || key.length === 0) return;

    var nodepath = Utils.getNodePathByNodeId(treeData, nodeid);

    var url = "/pages" + nodepath + "?op=listpage";

    fetch(url)
      .then((response) => response.text())
      .then((data) => {
        var childNodeData = JSON.parse(data);

        const fnUpdateData = (origin) => {
          return updateTreeData(origin, key, childNodeData);
        };

        setTreeData(fnUpdateData);

        if (selectedKey && selectedKey.length !== 0) {
          var selectedKeys = [];
          selectedKeys.push(selectedKey);

          // update UI selection
          setSelectedKeys(selectedKeys);

          // besides ui selection, also update data
          Globaldata.selectedTreeNode = selectedKey;
        }
      })
      .catch((error) => console.log("error is", error));
  }

  console.log("Antdtree::render()");
  if (
    Globaldata.treeDataShouldUpdate === "true" &&
    Globaldata.treeDataUpdateReason === "createpage"
  ) {
    // update tree data
    //updateEntireTreeData();
    updateNodeData(Globaldata.selectedTreeNode, Globaldata.createdNewPage);

    Globaldata.treeDataShouldUpdate = "false";
    Globaldata.treeDataUpdateReason = "";
    Globaldata.createdNewPage = "";
  }

  return (
    <Tree
      loadData={onLoadData}
      treeData={treeData}
      onSelect={onNodeSelected}
      selectedKeys={selectedKeys}
      ref={(component) => (Globaldata.treeInstance = component)}
    />
  );
}
