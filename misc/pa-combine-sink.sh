#!/bin/bash

NAME="streaming-desktop"

pacmd list-sinks | grep -q 'name: <'"$NAME"'>$'
if [ $? -lt 1 ]; then
  echo sink "$NAME" already present
  exit 0
fi


DEFAULT_OUTPUT=$(pacmd list-sinks | grep -A1 "* index" | grep -oP "<\K[^ >]+")
echo $DEFAULT_OUTPUT
pactl load-module module-combine-sink \
	sink_name="$NAME" slaves=$DEFAULT_OUTPUT \
	sink_properties=device.description="Streamed_by_OBS"
