#!/bin/sh

function load_config()
{
config=$CLAIMS_HOME/sbin/2-claims-conf/cluster-deploy.config
#master=`sed '/^master=/!d;s/.*=//' $config`
#slaves=`sed '/^slaves=/!d;s/.*=//' $config`
#local_disk_mode=`sed '/^local_disk_mode=/!d;s/.*=//' $config`
#data=`sed '/^data=/!d;s/.*=//' $config`
#deploypath=`sed '/^deploypath=/!d;s/.*=//' $config`
#user=`sed '/^user=/!d;s/.*=//' $config`
#client_listener_port=`sed '/^client_listener_port=/!d;s/.*=//' $config`
#logfilepath=`sed '/^logfilepath=/!d;s/.*=//' $config`
#runclaimsprocid=`sed '/^runclaimsprocid=/!d;s/.*=//' $config`
source $config
}

function print_config()
{
echo "=========cluster config start========"
echo "config :              [$config]"
echo "master :              [$master]"
echo "slaves :              [$slaves]"
echo "deploypath :          [$deploypath]"
echo "user :                [$user]"
echo "local_disk_mode :     [$local_disk_mode]"
echo "data :                [$data]"
echo "client_listener_port :[$client_listener_port]"
echo "logfilepath :         [$logfilepath]"
echo "runclaimsprocid :     [$runclaimsprocid]"
echo "=========cluster config end=========="
}

load_config
#print_config
