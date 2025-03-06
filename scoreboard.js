// scoreboard.js - Real-time Leaderboard using Firestore

// Difficulty ranking for sorting (higher value = higher rank)
const difficultyRank = { easy: 1, medium: 2, hard: 3, infinity: 4 };

// Global array to hold aggregated leaderboard data
let leaderboardData = [];
let leaderboardLoaded = false;  // flag to indicate initial data loaded

// Setup Firestore listener for the userScores collection (real-time updates)
firebase.firestore().collection('userScores')
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

        // (Optional) Limit to top 10 players for display/performance
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

// Utility function to get the current leaderboard data (for integration with UI)
function getLeaderboardData() {
    return leaderboardData;
}

// (Optional) You can also override the existing getAllPlayerScores to use the real-time data:
function getAllPlayerScores() {
    // Return a promise that resolves immediately with the latest data
    return Promise.resolve(leaderboardData);
}

// The above override ensures that any existing code calling getAllPlayerScores().then(...) 
// will receive the up-to-date leaderboardData without making a new database query. 
// This integrates with the current drawHighScores implementation seamlessly.

// Explicitly expose the function to the global scope
window.getAllPlayerScores = getAllPlayerScores;
