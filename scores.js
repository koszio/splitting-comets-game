// Firebase configuration is now in auth.js
// Use the firebase instance that was already initialized in auth.js
const db = firebase.firestore();

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