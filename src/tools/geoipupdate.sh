#!/bin/bash

# This script is a helper script for the irc2sql module.
# It downloads the configured geoip databases and inserts
# them into existing mysql tables. The tables are created
# by the irc2sql module on the first load.

# Don't forget to rename this file or your changes
# will be overwritte on the next 'make install'

############################
# Config
############################


geoip_database="country" # available options: "country" and "city"
mysql_host="localhost"
mysql_user="anope"
mysql_password="anope"
mysql_database="anope"
prefix="anope_"
die="yes"

###########################

# The GeoIP data is created by MaxMind, available from www.maxmind.com.
geoip_country_source="http://geolite.maxmind.com/download/geoip/database/GeoIPCountryCSV.zip"
geoip_city_source="http://geolite.maxmind.com/download/geoip/database/GeoLiteCity_CSV/GeoLiteCity-latest.zip"
geoip_region_source="http://dev.maxmind.com/static/csv/codes/maxmind/region.csv"

###########################
LOGIN="--host=$mysql_host --user=$mysql_user --password=$mysql_password $mysql_database"
PARAMS="--delete --local --fields-terminated-by=, --fields-enclosed-by=\" --lines-terminated-by=\n $LOGIN"

download() {
    local url=$1
    local desc=$2
    echo -n "   $desc     "
    wget --progress=dot $url 2>&1 | grep --line-buffered "%" | sed -u -e "s,\.,,g" | awk '{printf("\b\b\b\b%4s", $2)}'
    echo -ne " Done\n"
}


if test $die = "yes"; then
	echo "You have to edit and configure this script first."
	exit
fi


if test $geoip_database = "country"; then
	echo "Downloading..."
	download "$geoip_country_source" "Country Database:"
	echo "Unpacking..."
	unzip -jo GeoIPCountryCSV.zip
	rm GeoIPCountryCSV.zip
	echo "Converting to UFT-8..."
	iconv -f ISO-8859-1 -t UTF-8 GeoIPCountryWhois.csv -o $prefix"geoip_country.csv"
	rm GeoIPCountryWhois.csv
	echo "Importing..."
	mysqlimport --columns=@x,@x,start,end,countrycode,countryname $PARAMS $prefix"geoip_country.csv"
	rm $prefix"geoip_country.csv" $prefix"geoip_country6.csv"
	echo "Done..."
elif test $geoip_database = "city"; then
	echo "Downloading..."
	download "$geoip_city_source" "City Database:"
	download "$geoip_region_source" "Region Database:"
	echo "Unpacking..."
	unzip -jo GeoLiteCity-latest.zip
	rm GeoLiteCity-latest.zip
	echo "Converting to utf-8..."
	iconv -f ISO-8859-1 -t UTF-8 GeoLiteCity-Blocks.csv -o $prefix"geoip_city_blocks.csv"
	iconv -f ISO-8859-1 -t UTF-8 GeoLiteCity-Location.csv -o $prefix"geoip_city_location.csv"
	iconv -f ISO-8859-1 -t UTF-8 region.csv -o $prefix"geoip_city_region.csv"
	rm GeoLiteCity-Blocks.csv GeoLiteCity-Location.csv region.csv
	echo "Importing..."
	mysqlimport --columns=start,end,locID --ignore-lines=2 $PARAMS $prefix"geoip_city_blocks.csv"
	mysqlimport --columns=locID,country,region,city,@x,latitude,longitude,@x,areaCode --ignore-lines=2 $PARAMS $prefix"geoip_city_location.csv"
	mysqlimport --columns=country,region,regionname $PARAMS $prefix"geoip_city_region.csv"
	rm $prefix"geoip_city_blocks.csv" $prefix"geoip_city_location.csv" $prefix"geoip_city_region.csv" $prefix"geoip_country6.csv"
	echo "Done..."
fi
