// scoreboard.js - SQLite-based Leaderboard

// Difficulty ranking for sorting (higher value = higher rank)
const difficultyRank = { easy: 1, medium: 2, hard: 3, infinity: 4 };

// Global array to hold aggregated leaderboard data
let leaderboardData = [];
let leaderboardLoaded = false;  // flag to indicate initial data loaded

// Function to fetch and process leaderboard data from our local server
function refreshLeaderboard() {
  fetch('http://localhost:3000/scores')
    .then(response => response.json())
    .then(data => {
      // Temporary object to aggregate highest scores per player
      const playerScores = {};

      data.forEach(entry => {
        if (!entry.username || !entry.difficulty || typeof entry.score !== 'number') {
          // Skip any invalid entries
          return;
        }

        const username = entry.username;
        const difficulty = entry.difficulty.toLowerCase();
        const score = entry.score;

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

      // If we're currently on the high scores screen, redraw it
      if (window.gameState === "HIGH_SCORES" && typeof window.drawHighScores === 'function') {
        window.drawHighScores();
      }
    })
    .catch(error => {
      console.error("Error fetching leaderboard data:", error);
    });
}

// Set up polling to refresh the leaderboard data periodically
// This replaces the real-time listener functionality of Firebase
let leaderboardInterval;

function startLeaderboardPolling() {
  // Initial fetch
  refreshLeaderboard();
  
  // Then set up interval (every 10 seconds)
  leaderboardInterval = setInterval(refreshLeaderboard, 10000);
}

function stopLeaderboardPolling() {
  if (leaderboardInterval) {
    clearInterval(leaderboardInterval);
  }
}

// Start polling when the script loads
startLeaderboardPolling();

// Ensure polling stops when the page is unloaded
window.addEventListener('beforeunload', stopLeaderboardPolling);

// Utility function to get the current leaderboard data (for integration with UI)
function getLeaderboardData() {
  return leaderboardData;
}

// Function to manually refresh data (e.g., when entering scoreboard screen)
function refreshLeaderboardNow() {
  return refreshLeaderboard();
}

// Compatible API with original implementation
function getAllPlayerScores() {
  // If we already have data loaded, return it immediately
  if (leaderboardLoaded) {
    return Promise.resolve(leaderboardData);
  }
  
  // Otherwise, refresh and then return the data
  return new Promise((resolve) => {
    refreshLeaderboard();
    // Check every 100ms if data has loaded
    const checkInterval = setInterval(() => {
      if (leaderboardLoaded) {
        clearInterval(checkInterval);
        resolve(leaderboardData);
      }
    }, 100);
    
    // Timeout after 5 seconds
    setTimeout(() => {
      clearInterval(checkInterval);
      resolve(leaderboardData);
    }, 5000);
  });
}