#!/usr/bin/bash
#run in cygwin on windows

TARGET_PATH=/d/web_proj/src/cwf_backend/webapps/tslserver/server

#rebuild frontend
rm -rf ./build
yarn build

#delete files in target path

rm -rf ${TARGET_PATH}/tinymce
rm -rf ${TARGET_PATH}/static

rm ${TARGET_PATH}/asset-manifest.json
rm ${TARGET_PATH}/favicon.ico
rm ${TARGET_PATH}/index.html
rm ${TARGET_PATH}/logo192.png
rm ${TARGET_PATH}/logo512.png
rm ${TARGET_PATH}/manifest.json
rm ${TARGET_PATH}/robots.txt

# copy built files to target path

cp -rf ./build/* ${TARGET_PATH}