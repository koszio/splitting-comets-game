#!/bin/bash

# Start the local SQLite server
echo "Starting the local SQLite server..."
node server.js &
SERVER_PID=$!

# Sleep briefly to let the server start
sleep 2

# Determine the correct way to open a browser based on the OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    open "http://localhost:3000"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    xdg-open "http://localhost:3000" || sensible-browser "http://localhost:3000" || x-www-browser "http://localhost:3000"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    # Windows with Git Bash or similar
    start "http://localhost:3000"
else
    echo "Please open http://localhost:3000 in your browser"
fi

echo "Local game server running on http://localhost:3000"
echo "Press Ctrl+C to stop the server"

# Wait until user presses Ctrl+C
trap "kill $SERVER_PID; echo 'Server stopped.'; exit 0" INT
wait $SERVER_PID