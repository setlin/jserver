#!/bin/sh

filepath=$(cd "$(dirname "$0")"; pwd)

start()
{
    scripts=$1
    bin=$2
    nohup $filepath/$scripts/$bin >$filepath/$scripts/nohup.out &
    echo "开启 $filepath/$scripts/$bin "
    echo "日志 $filepath/$scripts/nohup.out."
}

stop()
{
    pid=`ps -ef| grep "$filepath"/"$1"/"$2" | grep -v grep | awk '{print $2}' `
    if [ "$pid"x != x ]; then
        kill -9 $pid
        echo "关闭" "$filepath"/"$1"/"$2" $pid
    fi
}

## 关闭网关
stopgw()
{
    stop "gatewayserver/src" "JSGateway"
}

## 关闭服务器
stopsrv()
{
    stop "logicserver/src" "JSServer"
}

## 开启服务器
startsrv()
{
    ## 读取服务器配置
    start "logicserver/src" "JSServer"
}

## 开启网关
startgw()
{
    ## 读取网关配置
    start "gatewayserver/src" "JSGateway" $id
}

case "$1" in
    all)
        stopgw;
        stopsrv;
        startgw;
        startsrv;
        ;;
esac


echo ""
echo ""
echo "ServerList"
echo "--------"
sleep 1
ps -ef | grep `pwd` | grep -v grep
echo "--------"
