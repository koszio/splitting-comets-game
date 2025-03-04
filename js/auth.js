// Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCahej0453N9l26zjuHkXbGyMTyFTgJcw0",
  authDomain: "splittingcomets.firebaseapp.com",
  projectId: "splittingcomets",
  storageBucket: "splittingcomets.firebasestorage.app",
  messagingSenderId: "280991593341",
  appId: "1:280991593341:web:b334e772452e026fcf1214",
  measurementId: "G-18ESZSW9ME"
};

// Initialize Firebase if not already initialized
if (!firebase.apps.length) {
  firebase.initializeApp(firebaseConfig);
}

const auth = firebase.auth();
const db = firebase.firestore();

// User state to be used across the application
let currentUser = null;

// Function to register a new user
function registerUser(email, password, username) {
  const errorMessage = document.getElementById('error-message');
  errorMessage.textContent = '';
  
  // Create new user account
  auth.createUserWithEmailAndPassword(email, password)
    .then((userCredential) => {
      // User account created successfully
      const user = userCredential.user;
      
      // Add user profile to Firestore
      return db.collection('users').doc(user.uid).set({
        username: username,
        email: email,
        createdAt: firebase.firestore.FieldValue.serverTimestamp()
      })
      .then(() => {
        // Update user profile display name
        return user.updateProfile({
          displayName: username
        });
      })
      .then(() => {
        // Redirect to login page
        window.location.href = 'login.html?registered=true';
      });
    })
    .catch((error) => {
      // Handle errors
      errorMessage.textContent = error.message;
      console.error("Registration error:", error);
    });
}

// Function to login a user
function loginUser(email, password) {
  const errorMessage = document.getElementById('error-message');
  errorMessage.textContent = '';
  
  auth.signInWithEmailAndPassword(email, password)
    .then((userCredential) => {
      // User logged in successfully
      const user = userCredential.user;
      
      // Store user info in localStorage for persistence
      localStorage.setItem('userLoggedIn', 'true');
      
      // Redirect to game
      window.location.href = '../index.html';
    })
    .catch((error) => {
      // Handle errors
      errorMessage.textContent = error.message;
      console.error("Login error:", error);
    });
}

// Function to logout a user
function logoutUser() {
  // Check if the game is in playing state
  if (window.gameState === "playing") {
    console.log("Cannot logout during gameplay");
    alert("Cannot logout during gameplay. Please pause or exit the game first.");
    return Promise.resolve(false);
  } 
  
  console.log("Current game state:", window.gameState);
  
  return auth.signOut()
    .then(() => {
      // Clear user info from localStorage
      localStorage.removeItem('userLoggedIn');
      currentUser = null;
      
      // Update UI elements in the game
      updateAuthUI(false);
      
      return true;
    })
    .catch((error) => {
      console.error("Logout error:", error);
      return false;
    });
}

// Function to check if user is logged in
function checkUserLoggedIn() {
  return new Promise((resolve) => {
    auth.onAuthStateChanged((user) => {
      if (user) {
        // User is signed in
        currentUser = user;
        db.collection('users').doc(user.uid).get()
          .then((doc) => {
            if (doc.exists) {
              const userData = doc.data();
              currentUser.username = userData.username;
            }
            updateAuthUI(true);
            resolve(true);
          })
          .catch((error) => {
            console.error("Error getting user data:", error);
            updateAuthUI(true);
            resolve(true);
          });
      } else {
        // User is signed out
        currentUser = null;
        updateAuthUI(false);
        resolve(false);
      }
    });
  });
}

// Function to update UI based on authentication state
function updateAuthUI(isLoggedIn) {
  const authButton = document.getElementById('auth-button');
  const userStatus = document.getElementById('user-status');
  
  if (!authButton || !userStatus) return; // Elements not found
  
  if (isLoggedIn && currentUser) {
    // Update UI for logged in user
    authButton.textContent = 'Logout';
    authButton.onclick = function() {
      // Check if the game is in playing state
      if (window.gameState && window.gameState === "playing") {
        // Don't allow logout during gameplay
        return false;
      } else {
        logoutUser();
      }
    };
    
    // Display username
    const displayName = currentUser.username || currentUser.displayName || currentUser.email;
    userStatus.textContent = `Logged in as: ${displayName}`;
    userStatus.style.color = "#4d9eff";
  } else {
    // Update UI for guest user
    authButton.textContent = 'Login';
    authButton.onclick = function() {
      // Check if the game is in playing state
      if (window.gameState && window.gameState === "playing") {
        // Don't allow login during gameplay
        return false;
      } else {
        window.location.href = 'auth/login.html';
      }
    };
    
    userStatus.textContent = 'Playing as Guest';
    userStatus.style.color = "#aaa";
  }
}

// Function to save a score associated with the current user
function saveUserScore(score, difficulty) {
  if (!currentUser) {
    console.log("Cannot save score: No user logged in");
    return Promise.resolve({ success: false, message: "Not logged in" });
  }
  
  const scoreData = {
    userId: currentUser.uid,
    username: currentUser.username || currentUser.displayName || "Anonymous",
    score: score,
    difficulty: difficulty,
    timestamp: firebase.firestore.FieldValue.serverTimestamp()
  };
  
  // Save to user's scores collection
  return db.collection('userScores').add(scoreData)
    .then(() => {
      console.log(`Score saved for user ${currentUser.uid}: ${score} points (${difficulty})`);
      
      // Also update high score if needed
      return updateHighScore(score, difficulty);
    })
    .catch((error) => {
      console.error("Error saving user score:", error);
      return { success: false, error: error.message };
    });
}

// Update high score function (compatible with the one in scores.js)
function updateHighScore(score, difficulty) {
  console.log(`Checking high score: ${score} points, difficulty: ${difficulty}`);
  
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
        const username = currentUser ? (currentUser.username || currentUser.displayName || "Anonymous") : "Guest";
        
        const scoreData = {
          score: score,
          difficulty: difficulty,
          userId: currentUser ? currentUser.uid : "guest",
          username: username,
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

// Initialize authentication on every page
document.addEventListener('DOMContentLoaded', function() {
  // Only run this on the game page, not on auth pages
  if (!window.location.pathname.includes('/auth/')) {
    checkUserLoggedIn();
  }
  
  // Check URL parameters for messages
  const urlParams = new URLSearchParams(window.location.search);
  if (urlParams.has('registered')) {
    const errorMessage = document.getElementById('error-message');
    if (errorMessage) {
      errorMessage.className = 'success-message';
      errorMessage.textContent = 'Registration successful! Please login.';
    }
  }
});