// scoreboard.js - Real-time Leaderboard using Firestore

// Difficulty ranking for sorting (higher value = higher rank)
const difficultyRank = { easy: 1, medium: 2, hard: 3, infinity: 4 };

// Global array to hold aggregated leaderboard data
let leaderboardData = [];
let leaderboardLoaded = false;  // flag to indicate initial data loaded
let leaderboardListener = null; // store reference to listener for detaching

// Function to setup the Firestore listener - only when needed
function setupLeaderboardListener() {
    // If already listening, don't set up another listener
    if (leaderboardListener) return;
    
    // Setup Firestore listener for the userScores collection (real-time updates)
    // Only get top 50 scores by score value to reduce data transfer
    leaderboardListener = firebase.firestore().collection('userScores')
        .orderBy('score', 'desc')
        .limit(50)
        .onSnapshot((querySnapshot) => {
            // Temporary object to aggregate highest scores per player
            const playerScores = {};

            querySnapshot.forEach(doc => {
                const data = doc.data();
                if (!data.username || !data.difficulty || typeof data.score !== 'number') {
                    // Skip any invalid entries
                    return;
                }

                const username = data.username;
                const difficulty = data.difficulty.toLowerCase();
                const score = data.score;

                // Initialize this player's record if not seen before
                if (!playerScores[username]) {
                    playerScores[username] = {
                        username: username,
                        // Start with 0 for each difficulty (0 will represent no score recorded yet)
                        scores: { easy: 0, medium: 0, hard: 0, infinity: 0 }
                    };
                }

                // Update the player's score for this difficulty if it's a new high score
                if (score > playerScores[username].scores[difficulty]) {
                    playerScores[username].scores[difficulty] = score;
                }
            });

            // Convert aggregated object to an array for sorting
            let playersArray = Object.values(playerScores);

            // Sort players by highest difficulty rank, then by score in that difficulty
            playersArray.sort((a, b) => {
                // Determine the highest difficulty achieved by player a
                let aMaxDiff = 'easy';
                if (a.scores.infinity > 0) {
                    aMaxDiff = 'infinity';
                } else if (a.scores.hard > 0) {
                    aMaxDiff = 'hard';
                } else if (a.scores.medium > 0) {
                    aMaxDiff = 'medium';
                } else if (a.scores.easy > 0) {
                    aMaxDiff = 'easy';
                }
                // Determine highest difficulty achieved by player b
                let bMaxDiff = 'easy';
                if (b.scores.infinity > 0) {
                    bMaxDiff = 'infinity';
                } else if (b.scores.hard > 0) {
                    bMaxDiff = 'hard';
                } else if (b.scores.medium > 0) {
                    bMaxDiff = 'medium';
                } else if (b.scores.easy > 0) {
                    bMaxDiff = 'easy';
                }

                // Compare difficulty rank first
                const diffCompare = difficultyRank[bMaxDiff] - difficultyRank[aMaxDiff];
                if (diffCompare !== 0) {
                    return diffCompare; // higher difficulty comes first
                }

                // If same highest difficulty, sort by the score in that difficulty (descending)
                return b.scores[bMaxDiff] - a.scores[aMaxDiff];
            });

            // Limit to top 10 players for display/performance
            leaderboardData = playersArray.slice(0, 10);
            leaderboardLoaded = true;

            // Trigger UI refresh when scoreboard is visible
            if (window.gameState === "highScores" && leaderboardLoaded) {
                // Force a redraw by changing game state temporarily and back
                if (typeof window.drawHighScores === 'function') {
                    window.drawHighScores();
                } else {
                    // If custom draw function not available, refresh scores in the game
                    if (typeof loadHighScores === 'function') {
                        loadHighScores();
                    }
                }
            }
        }, error => {
            console.error("Error fetching leaderboard data:", error);
        });
}

// Function to detach the listener when not needed
function detachLeaderboardListener() {
    if (leaderboardListener) {
        leaderboardListener();
        leaderboardListener = null;
    }
}

// Utility function to get the current leaderboard data (for integration with UI)
function getLeaderboardData() {
    // Make sure we have a listener set up if requesting data
    setupLeaderboardListener();
    return leaderboardData;
}

// Override the existing getAllPlayerScores to use the real-time data:
function getAllPlayerScores() {
    // Set up the listener if not already listening
    setupLeaderboardListener();
    
    // Return a promise that resolves immediately with the latest data
    return Promise.resolve(leaderboardData);
}

// Function to manage listeners based on game state
function manageLeaderboardListeners(gameState) {
    if (gameState === "highScores") {
        // Start listening when entering the high scores screen
        setupLeaderboardListener();
    } else if (leaderboardListener && gameState !== "highScores" && gameState !== "game") {
        // Detach when not in high scores or gameplay
        detachLeaderboardListener();
    }
}

// Listen for game state changes to manage the listener lifecycle
window.addEventListener('gameStateChange', function(e) {
    if (e && e.detail && e.detail.newState) {
        manageLeaderboardListeners(e.detail.newState);
    }
});

// Explicitly expose the functions to the global scope
window.getAllPlayerScores = getAllPlayerScores;
window.setupLeaderboardListener = setupLeaderboardListener;
window.detachLeaderboardListener = detachLeaderboardListener;
