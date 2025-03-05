// Firebase configuration is now in auth.js
// Use the firebase instance that was already initialized in auth.js
// DO NOT declare db again since it's declared in auth.js

// Function to check if the current score is a new high score and save it
function updateHighScore(score, difficulty) {
  console.log(`Checking high score: ${score} points, difficulty: ${difficulty}`);
  
  // Check if auth.js is loaded and use its function if available
  if (typeof window.saveUserScore === 'function' && typeof currentUser !== 'undefined' && currentUser) {
    console.log("Using auth.js saveUserScore function");
    return window.saveUserScore(score, difficulty);
  }
  
  // Fallback to original implementation if auth.js not loaded
  // Ensure difficulty is a valid string
  if (!difficulty || difficulty === "undefined") {
    difficulty = "medium"; // Default to medium difficulty
  }
  
  // Use a standardized document ID for the high score
  const highScoreId = `highscore_${difficulty}`;
  
  // Reference to the high score document
  const highScoreRef = db.collection('highscores').doc(highScoreId);
  
  // Get the current high score
  return highScoreRef.get()
    .then(doc => {
      let isNewHighScore = false;
      
      if (doc.exists) {
        const currentHighScore = doc.data().score;
        console.log(`Current high score for ${difficulty}: ${currentHighScore}`);
        
        // Check if this is a new high score
        if (score > currentHighScore) {
          isNewHighScore = true;
          console.log(`New high score! ${score} > ${currentHighScore}`);
        } else {
          console.log(`Not a new high score. ${score} <= ${currentHighScore}`);
          return { success: true, isNewHighScore: false, highScore: currentHighScore };
        }
      } else {
        // No high score exists yet for this difficulty
        isNewHighScore = true;
        console.log(`First high score for ${difficulty}: ${score}`);
      }
      
      // If it's a new high score, save it
      if (isNewHighScore) {
        // Try to get current user from auth.js
        const username = (typeof currentUser !== 'undefined' && currentUser) 
          ? (currentUser.username || currentUser.displayName || "Anonymous") 
          : "Guest";
        
        const userId = (typeof currentUser !== 'undefined' && currentUser)
          ? currentUser.uid
          : "guest";
          
        const scoreData = {
          score: score,
          difficulty: difficulty,
          username: username,
          userId: userId,
          timestamp: firebase.firestore.FieldValue.serverTimestamp(),
          dateAchieved: new Date().toISOString()
        };
        
        // Also save the score to the user scores collection for the leaderboard
        saveScore(username, score, difficulty);
        
        return highScoreRef.set(scoreData)
          .then(() => {
            console.log(`High score updated: ${score}`);
            return { success: true, isNewHighScore: true, highScore: score };
          });
      }
    })
    .catch(error => {
      console.error("Error updating high score:", error);
      return { success: false, error: error.message };
    });
}

// Function to save a user score to the userScores collection (for leaderboard)
function saveScore(username, score, difficulty) {
  console.log("Saving score:", { username, score, difficulty });
  
  const scoreData = {
    username: username,
    userId: firebase.auth().currentUser?.uid || 'anonymous',
    score: Number(score), // Ensure score is a number
    difficulty: difficulty.toLowerCase(), // Ensure consistent case
    timestamp: firebase.firestore.FieldValue.serverTimestamp()
  };

  return firebase.firestore()
    .collection('userScores')
    .add(scoreData)
    .then(docRef => {
      console.log("Score saved successfully:", docRef.id);
      return { success: true };
    })
    .catch(error => {
      console.error("Error saving score:", error);
      return { success: false, error: error.message };
    });
}

// Function to get the current high score for a difficulty level
function getHighScore(difficulty = "medium") {
  // Check if auth.js is loaded and use its function if available
  if (typeof window.getHighScore === 'function' && window.getHighScore !== getHighScore) {
    console.log("Using auth.js getHighScore function");
    return window.getHighScore(difficulty);
  }
  
  const highScoreRef = db.collection('highscores').doc(`highscore_${difficulty}`);
  
  return highScoreRef.get()
    .then(doc => {
      if (doc.exists) {
        const highScoreData = doc.data();
        console.log(`Retrieved high score for ${difficulty}: ${highScoreData.score}`);
        return { 
          score: highScoreData.score, 
          date: highScoreData.dateAchieved,
          username: highScoreData.username || "Anonymous"
        };
      } else {
        console.log(`No high score found for ${difficulty}`);
        return { score: 0, date: null, username: "None" };
      }
    })
    .catch(error => {
      console.error("Error getting high score:", error);
      return { score: 0, date: null, username: "Error", error: error.message };
    });
}

// Function to get all player scores for the leaderboard
function getAllPlayerScores() {
  console.log("Getting all player scores...");
  
  return db.collection('userScores')
    .get()
    .then(snapshot => {
      console.log("Retrieved scores, count:", snapshot.size);
      
      // Group scores by player and keep highest score per difficulty
      const playerScores = {};
      
      snapshot.forEach(doc => {
        const data = doc.data();
        
        // Skip invalid data
        if (!data.username || !data.difficulty || typeof data.score !== 'number') {
          console.warn("Skipping invalid score data:", data);
          return;
        }
        
        const username = data.username;
        const difficulty = data.difficulty.toLowerCase();
        const score = data.score;
        
        // Initialize player entry if needed
        if (!playerScores[username]) {
          playerScores[username] = {
            username: username,
            scores: {
              easy: 0,
              medium: 0,
              hard: 0,
              infinity: 0
            }
          };
        }
        
        // Update score if higher than existing
        if (score > playerScores[username].scores[difficulty]) {
          playerScores[username].scores[difficulty] = score;
        }
      });
      
      // Convert to array and sort by total score
      const sortedPlayers = Object.values(playerScores).sort((a, b) => {
        const totalA = a.scores.easy + a.scores.medium + a.scores.hard + a.scores.infinity;
        const totalB = b.scores.easy + b.scores.medium + b.scores.hard + b.scores.infinity;
        return totalB - totalA;
      });
      
      // Limit to 10 players for better performance
      return sortedPlayers.slice(0, 10);
    })
    .catch(error => {
      console.error("Error fetching player scores:", error);
      return [];
    });
}

// Expose the function globally so it can be accessed from index.html
window.getAllPlayerScores = getAllPlayerScores;