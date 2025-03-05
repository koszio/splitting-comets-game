// Local SQLite-based score tracking
// The actual database operations happen in the server.js Node backend

// Function to check if the current score is a new high score and save it
function updateHighScore(score, difficulty) {
  console.log(`Checking high score: ${score} points, difficulty: ${difficulty}`);
  
  // Ensure difficulty is a valid string
  if (!difficulty || difficulty === "undefined") {
    difficulty = "medium"; // Default to medium difficulty
  }
  
  // Get username from auth.js if available, otherwise use Guest
  const username = (typeof currentUser !== 'undefined' && currentUser) 
    ? (currentUser.username || currentUser.displayName || "Anonymous") 
    : "Guest";
  
  // Send the score to our local server
  return fetch('http://localhost:3000/scores', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ 
      username, 
      difficulty, 
      score 
    })
  })
  .then(response => response.json())
  .then(result => {
    console.log('Score submission result:', result);
    
    // Since the server handles determining if it's a high score,
    // we'll fetch the current high score to see if our score is now the highest
    return getHighScore(difficulty)
      .then(highScoreData => {
        const isNewHighScore = highScoreData.score === score;
        console.log(`Is new high score? ${isNewHighScore}`);
        return { 
          success: true, 
          isNewHighScore, 
          highScore: highScoreData.score 
        };
      });
  })
  .catch(error => {
    console.error("Error updating high score:", error);
    return { success: false, error: error.message };
  });
}

// Function to get the current high score for a difficulty level
function getHighScore(difficulty = "medium") {
  // Ensure difficulty is standardized
  difficulty = difficulty.toLowerCase();
  
  return fetch('http://localhost:3000/scores')
    .then(response => response.json())
    .then(data => {
      // Filter to find the highest score for the requested difficulty
      const filteredScores = data.filter(score => score.difficulty === difficulty);
      
      if (filteredScores.length > 0) {
        // Sort by score (descending)
        filteredScores.sort((a, b) => b.score - a.score);
        const highScoreData = filteredScores[0];
        
        console.log(`Retrieved high score for ${difficulty}: ${highScoreData.score}`);
        return { 
          score: highScoreData.score, 
          date: highScoreData.timestamp,
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