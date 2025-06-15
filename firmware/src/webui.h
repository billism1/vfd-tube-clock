#include <Arduino.h>

String getWebUI(bool isDisplayTimeMode, String customText, bool isFlashMessageMode, String formattedTime)
{
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<title>GOSUDARSTVENNY VFD CLOCK CONTROL - SSSR</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  
  // Import Soviet-style font
  html += "@import url('https://fonts.googleapis.com/css2?family=Russo+One&family=Rubik:wght@400;700&display=swap');";
  
  // Main styling with Soviet theme
  html += "body { ";
  html += "  font-family: 'Rubik', 'Arial', sans-serif; ";
  html += "  margin: 0; ";
  html += "  background: linear-gradient(135deg, #8B0000 0%, #DC143C 25%, #8B0000 50%, #B22222 75%, #8B0000 100%); ";
  html += "  background-attachment: fixed; ";
  html += "  min-height: 100vh; ";
  html += "  color: #FFFFFF; ";
  html += "}";
  
  // Add Soviet pattern overlay
  html += "body::before { ";
  html += "  content: ''; ";
  html += "  position: fixed; ";
  html += "  top: 0; left: 0; right: 0; bottom: 0; ";
  html += "  background-image: ";
  html += "    radial-gradient(circle at 20% 20%, rgba(255,255,0,0.1) 2px, transparent 2px), ";
  html += "    radial-gradient(circle at 80% 80%, rgba(255,255,0,0.1) 2px, transparent 2px); ";
  html += "  background-size: 50px 50px; ";
  html += "  pointer-events: none; ";
  html += "  z-index: -1; ";
  html += "}";
  
  // Top banner with Soviet styling - FIXED: Increased right padding
  html += ".soviet-banner { ";
  html += "  background: linear-gradient(90deg, #FFD700 0%, #FFA500 50%, #FFD700 100%); ";
  html += "  color: #8B0000; ";
  html += "  text-align: center; ";
  html += "  padding: 15px; ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  font-weight: bold; ";
  html += "  font-size: 14px; ";
  html += "  letter-spacing: 2px; ";
  html += "  text-transform: uppercase; ";
  html += "  border-bottom: 4px solid #8B0000; ";
  html += "  box-shadow: 0 4px 8px rgba(0,0,0,0.3); ";
  html += "  position: relative; ";
  html += "  padding-right: 160px; "; // INCREASED from 120px to 160px
  html += "}";
  
  // Main container
  html += ".container { ";
  html += "  max-width: 600px; ";
  html += "  margin: 20px auto; ";
  html += "  background: linear-gradient(145deg, #2F2F2F 0%, #1C1C1C 100%); ";
  html += "  padding: 30px; ";
  html += "  border: 3px solid #FFD700; ";
  html += "  border-radius: 0; ";
  html += "  box-shadow: ";
  html += "    0 0 20px rgba(255,215,0,0.3), ";
  html += "    inset 0 0 20px rgba(0,0,0,0.5), ";
  html += "    0 8px 32px rgba(0,0,0,0.4); ";
  html += "  position: relative; ";
  html += "}";
  
  // Add industrial corner brackets
  html += ".container::before, .container::after { ";
  html += "  content: ''; ";
  html += "  position: absolute; ";
  html += "  width: 20px; height: 20px; ";
  html += "  border: 3px solid #FFD700; ";
  html += "}";
  html += ".container::before { ";
  html += "  top: -3px; left: -3px; ";
  html += "  border-right: none; border-bottom: none; ";
  html += "}";
  html += ".container::after { ";
  html += "  bottom: -3px; right: -3px; ";
  html += "  border-left: none; border-top: none; ";
  html += "}";
  
  // Main heading
  html += "h1 { ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  color: #FFD700; ";
  html += "  text-align: center; ";
  html += "  font-size: 24px; ";
  html += "  margin: 0 0 30px 0; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 3px; ";
  html += "  text-shadow: ";
  html += "    2px 2px 0px #8B0000, ";
  html += "    4px 4px 8px rgba(0,0,0,0.8); ";
  html += "  border-bottom: 2px solid #8B0000; ";
  html += "  padding-bottom: 15px; ";
  html += "}";
  
  // Status panel
  html += ".status { ";
  html += "  background: linear-gradient(135deg, #8B0000 0%, #A52A2A 100%); ";
  html += "  padding: 20px; ";
  html += "  border: 2px solid #FFD700; ";
  html += "  margin: 25px 0; ";
  html += "  box-shadow: ";
  html += "    inset 0 0 10px rgba(0,0,0,0.5), ";
  html += "    0 4px 8px rgba(0,0,0,0.3); ";
  html += "  position: relative; ";
  html += "}";
  
  // Add industrial rivets to status panel
  html += ".status::before { ";
  html += "  content: '● ● ● ● ● ● ● ● ● ●'; ";
  html += "  position: absolute; ";
  html += "  top: 5px; left: 10px; right: 10px; ";
  html += "  color: #666; ";
  html += "  font-size: 8px; ";
  html += "  letter-spacing: 15px; ";
  html += "}";
  
  // Control groups
  html += ".control-group { ";
  html += "  margin: 25px 0; ";
  html += "  padding: 20px; ";
  html += "  background: linear-gradient(145deg, #3C3C3C 0%, #2A2A2A 100%); ";
  html += "  border: 1px solid #555; ";
  html += "  box-shadow: inset 0 0 10px rgba(0,0,0,0.5); ";
  html += "}";
  
  // Labels
  html += "label { ";
  html += "  display: block; ";
  html += "  margin-bottom: 10px; ";
  html += "  font-weight: bold; ";
  html += "  color: #FFD700; ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 1px; ";
  html += "  font-size: 14px; ";
  html += "}";
  
  // Text inputs
  html += "input[type='text'] { ";
  html += "  width: 100%; ";
  html += "  padding: 15px; ";
  html += "  border: 2px solid #8B0000; ";
  html += "  background: #1A1A1A; ";
  html += "  color: #49D8AE; ";
  html += "  box-sizing: border-box; ";
  html += "  font-family: 'Digital-7', 'Orbitron', 'Consolas', monospace; ";
  html += "  font-size: 16px; ";
  html += "  font-weight: bold; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 2px; "; // Added for better 7-segment appearance
  html += "  box-shadow: ";
  html += "    inset 0 0 10px rgba(0,0,0,0.8), ";
  html += "    0 0 5px rgba(139,0,0,0.5); ";
  html += "}";
  
  html += "input[type='text']:focus { ";
  html += "  outline: none; ";
  html += "  border-color: #FFD700; ";
  html += "  box-shadow: ";
  html += "    inset 0 0 10px rgba(0,0,0,0.8), ";
  html += "    0 0 10px rgba(255,215,0,0.8); ";
  html += "}";
  
  // Soviet-style buttons
  html += "button { ";
  html += "  background: linear-gradient(145deg, #8B0000 0%, #DC143C 50%, #8B0000 100%); ";
  html += "  color: #FFD700; ";
  html += "  padding: 15px 25px; ";
  html += "  border: 2px solid #FFD700; ";
  html += "  cursor: pointer; ";
  html += "  margin: 8px 0; ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  font-size: 14px; ";
  html += "  font-weight: bold; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 1px; ";
  html += "  box-shadow: ";
  html += "    0 4px 8px rgba(0,0,0,0.4), ";
  html += "    inset 0 1px 0 rgba(255,255,255,0.2); ";
  html += "  position: relative; ";
  html += "  overflow: hidden; ";
  html += "}";
  
  html += "button:hover { ";
  html += "  background: linear-gradient(145deg, #A52A2A 0%, #FF6347 50%, #A52A2A 100%); ";
  html += "  box-shadow: ";
  html += "    0 6px 12px rgba(0,0,0,0.6), ";
  html += "    inset 0 1px 0 rgba(255,255,255,0.3), ";
  html += "    0 0 15px rgba(255,215,0,0.4); ";
  html += "  transform: translateY(-2px); ";
  html += "}";
  
  html += "button:active { ";
  html += "  transform: translateY(0); ";
  html += "  box-shadow: ";
  html += "    0 2px 4px rgba(0,0,0,0.4), ";
  html += "    inset 0 0 10px rgba(0,0,0,0.3); ";
  html += "}";
  
  // Wide buttons
  html += ".wide-btn { ";
  html += "  width: 100%; ";
  html += "  font-size: 16px; ";
  html += "  padding: 18px; ";
  html += "}";
  
  // Current mode styling
  html += ".current-mode { ";
  html += "  font-size: 20px; ";
  html += "  font-weight: bold; ";
  html += "  color: #FFD700; ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 2px; ";
  html += "  text-shadow: 2px 2px 4px rgba(0,0,0,0.8); ";
  html += "  margin-bottom: 15px; ";
  html += "}";
  
  // Status text
  html += ".status div { ";
  html += "  margin: 10px 0; ";
  html += "  font-size: 16px; ";
  html += "  font-weight: bold; ";
  html += "  color: #FFFFFF; ";
  html += "  text-shadow: 1px 1px 2px rgba(0,0,0,0.8); ";
  html += "}";
  
  // Add blinking effect for current time
  html += "@keyframes blink { ";
  html += "  0%, 50% { opacity: 1; } ";
  html += "  51%, 100% { opacity: 0.7; } ";
  html += "}";
  
  html += ".time-display { ";
  html += "  animation: blink 2s infinite; ";
  html += "  color: #00FF00; ";
  html += "  font-family: 'Courier New', monospace; ";
  html += "}";
  
  // Language toggle button - Fixed positioning
  html += ".language-toggle { ";
  html += "  position: absolute; ";
  html += "  top: 15px; ";
  html += "  right: 15px; ";
  html += "  background: linear-gradient(145deg, #FFD700 0%, #FFA500 100%); ";
  html += "  color: #8B0000; ";
  html += "  padding: 8px 12px; ";
  html += "  border: 2px solid #8B0000; ";
  html += "  cursor: pointer; ";
  html += "  font-family: 'Russo One', sans-serif; ";
  html += "  font-size: 11px; ";
  html += "  font-weight: bold; ";
  html += "  text-transform: uppercase; ";
  html += "  letter-spacing: 1px; ";
  html += "  box-shadow: ";
  html += "    0 4px 8px rgba(0,0,0,0.4), ";
  html += "    inset 0 1px 0 rgba(255,255,255,0.3); ";
  html += "  transition: all 0.3s ease; ";
  html += "  z-index: 10; ";
  html += "}";
  
  html += ".language-toggle:hover { ";
  html += "  background: linear-gradient(145deg, #FFFF00 0%, #FFD700 100%); ";
  html += "  transform: translateY(-2px); ";
  html += "  box-shadow: ";
  html += "    0 6px 12px rgba(0,0,0,0.6), ";
  html += "    inset 0 1px 0 rgba(255,255,255,0.4); ";
  html += "}";
  
  html += ".language-toggle:active { ";
  html += "  transform: translateY(0); ";
  html += "  box-shadow: ";
  html += "    0 2px 4px rgba(0,0,0,0.4), ";
  html += "    inset 0 0 8px rgba(0,0,0,0.2); ";
  html += "}";
  
  // Mobile responsiveness - UPDATED: Adjusted padding values
  html += "@media (max-width: 768px) { ";
  html += "  .soviet-banner { ";
  html += "    font-size: 12px; ";
  html += "    padding: 10px 5px; ";
  html += "    padding-right: 120px; "; // Reduced for mobile
  html += "    letter-spacing: 1px; ";
  html += "  } ";
  html += "  .language-toggle { ";
  html += "    top: 10px; ";
  html += "    right: 10px; ";
  html += "    padding: 6px 8px; ";
  html += "    font-size: 10px; ";
  html += "  } ";
  html += "  .container { ";
  html += "    margin: 10px; ";
  html += "    padding: 20px; ";
  html += "  } ";
  html += "  h1 { ";
  html += "    font-size: 20px; ";
  html += "    letter-spacing: 2px; ";
  html += "  } ";
  html += "} ";
  
  html += "@media (max-width: 480px) { ";
  html += "  .soviet-banner { ";
  html += "    font-size: 10px; ";
  html += "    padding: 8px 5px; ";
  html += "    padding-right: 100px; "; // Further reduced for small screens
  html += "    letter-spacing: 0px; ";
  html += "  } ";
  html += "  .language-toggle { ";
  html += "    top: 8px; ";
  html += "    right: 8px; ";
  html += "    padding: 5px 6px; ";
  html += "    font-size: 9px; ";
  html += "  } ";
  html += "  h1 { ";
  html += "    font-size: 18px; ";
  html += "    letter-spacing: 1px; ";
  html += "  } ";
  html += "  .current-mode { ";
  html += "    font-size: 16px; ";
  html += "  } ";
  html += "} ";
  
  html += "</style></head><body>";

  html += "<button class='language-toggle' onclick='toggleLanguage()' id='langToggle'>🇺🇸 ENGLISH</button>";

  // Soviet banner
  html += "<div class='soviet-banner' id='banner'>";
  html += "GOSUDARSTVENNY KONTROL VREMENI • PROLETARII VSEKH STRAN, SOEDINYAYTES!";
  html += "</div>";
  
  html += "<div class='container'>";
  html += "<h1 id='title'>☭ IV-18 VFD CHASY ☭<br><small style='font-size:12px; letter-spacing:1px;' id='subtitle'>UPRAVLENIE VREMENEM DLYA NARODA</small></h1>";
  
  html += "<div class='status'>";
  html += "<div class='current-mode' id='currentMode'>TEKUSHCHY REZHIM: " + String(isDisplayTimeMode ? "OTOBRAZHENIYE VREMENI" : "POLZOVATELSKY TEKST") + "</div>";
  
  // Show flash message status
  html += "<div id='flashStatus'>MIGAYUSHCHIYE SOOBSHCHENIYA: " + String(isFlashMessageMode ? "VKLYUCHENO" : "OTKLYUCHENO") + "</div>";

  if (!isDisplayTimeMode)
  {
    html += "<div class='time-display' id='comradeText'>TEKST TOVARISHCHA: \"" + customText + "\"</div>";
  }

  // Only show time display when in time mode (isDisplayTimeMode == true)
  if (isDisplayTimeMode)
  {
    html += "<div class='time-display' id='timeDisplay'>MOSKOVSKOYE VREMYA: " + formattedTime + "</div>";
  }
  
  html += "</div>";
  
  html += "<div class='control-group'>";
  html += "<button class='wide-btn' onclick='toggleMode()' id='toggleBtn'>";
  html += "☭ " + String(isDisplayTimeMode ? "PEREKLYUCHIT NA TEKST" : "PEREKLYUCHIT NA VREMYA") + " ☭";
  html += "</button>";
  html += "</div>";
  
  html += "<div class='control-group'>";
  html += "<button class='wide-btn' onclick='toggleFlashMessages()' id='flashToggleBtn'>";
  html += "☭ " + String(isFlashMessageMode ? "OTKLYUCHIT MIGANIE" : "VKLYUCHIT MIGANIE") + " ☭";
  html += "</button>";
  html += "</div>";
  
  html += "<div class='control-group'>";
  html += "<label id='messageLabel'>☭ SOOBSHCHENIYE DLYA NARODA (8 simvolov maksimum):</label>";
  html += "<input type='text' id='customTextInput' maxlength='8' value='" + customText.substring(0, 8) + "' placeholder='VVESTI TEKST TOVARISHCHA...'>";
  html += "<button class='wide-btn' onclick='setText()' id='setTextBtn'>☭ USTANOVIT TEKST REVOLYUTSII ☭</button>";
  html += "</div>";
  
  html += "</div>";
  
  html += "<script>";
  html += "let currentDisplayMode = " + String(isDisplayTimeMode ? "true" : "false") + ";";
  html += "let currentFlashMode = " + String(isFlashMessageMode ? "true" : "false") + ";";

  // Initialize language from localStorage, default to Russian if not set
  html += "let isRussian = localStorage.getItem('vfd_language') !== 'english';";
  html += "  ";

  html += "const translations = {";
  html += "  russian: {";
  html += "    banner: '☭ GOSUDARSTVENNY KONTROL VREMENI • PROLETARII VSEKH STRAN, SOEDINYAYTES! ☭',";
  html += "    title: '☭ IV-18 VFD CHASY ☭',";
  html += "    subtitle: 'UPRAVLENIE VREMENEM DLYA NARODA',";
  html += "    currentMode: 'TEKUSHCHY REZHIM: ',";
  html += "    timeDisplay: 'VREMENI',";
  html += "    customText: 'POLZOVATELSKY TEKST',";
  html += "    comradeText: 'TEKST TOVARISHCHA: ',";
  html += "    moscowTime: 'MOSKOVSKOYE VREMYA: ',";
  html += "    flashMessages: 'MIGAYUSHCHIYE SOOBSHCHENIYA: ',";
  html += "    enabled: 'VKLYUCHENO',";
  html += "    disabled: 'OTKLYUCHENO',";
  html += "    switchToText: 'PEREKLYUCHIT NA TEKST',";
  html += "    switchToTime: 'PEREKLYUCHIT NA VREMYA',";
  html += "    enableFlash: 'VKLYUCHIT MIGANIE',";
  html += "    disableFlash: 'OTKLYUCHIT MIGANIE',";
  html += "    messageLabel: 'SOOBSHCHENIYE DLYA NARODA (8 simvolov maksimum):',";
  html += "    placeholder: 'VVESTI TEKST TOVARISHCHA...',";
  html += "    setButton: 'USTANOVIT TEKST REVOLYUTSII',";
  html += "    langButton: '🇺🇸 ENGLISH'";
  html += "  },";
  html += "  english: {";
  html += "    banner: 'STATE TIME CONTROL • WORKERS OF THE WORLD, UNITE!',";
  html += "    title: 'IV-18 VFD CLOCK',";
  html += "    subtitle: 'TIME MANAGEMENT FOR THE PEOPLE',";
  html += "    currentMode: 'CURRENT MODE: ',";
  html += "    timeDisplay: 'TIME',";
  html += "    customText: 'CUSTOM TEXT',";
  html += "    comradeText: 'CITIZEN TEXT: ',";
  html += "    moscowTime: 'CURRENT TIME: ',";
  html += "    flashMessages: 'FLASH MESSAGES: ',";
  html += "    enabled: 'ENABLED',";
  html += "    disabled: 'DISABLED',";
  html += "    switchToText: 'SWITCH TO CUSTOM TEXT',";
  html += "    switchToTime: 'SWITCH TO TIME DISPLAY',";
  html += "    enableFlash: 'ENABLE FLASH MESSAGES',";
  html += "    disableFlash: 'DISABLE FLASH MESSAGES',";
  html += "    messageLabel: 'MESSAGE FOR THE PEOPLE (8 characters max):',";
  html += "    placeholder: 'ENTER YOUR TEXT...',";
  html += "    setButton: 'SET FREEDOM TEXT',";
  html += "    langButton: '🇷🇺 РУССКИЙ'";
  html += "  }";
  html += "};";

  // Initialize language on page load
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  updateLanguage();";
  html += "});";

  html += "function toggleLanguage() {";
  html += "  isRussian = !isRussian;";

  // Save language preference to localStorage";
  html += "  localStorage.setItem('vfd_language', isRussian ? 'russian' : 'english');";
  
  html += "  updateLanguage();";
  html += "}";
  html += "";
  html += "function updateLanguage() {";
  html += "  const lang = isRussian ? translations.russian : translations.english;";
  html += "  ";

  // Update banner
  html += "  document.getElementById('banner').textContent = lang.banner;";
  html += "  ";

  // Update title and subtitle
  html += "  document.getElementById('title').innerHTML = lang.title + '<br><small style=\\\"font-size:12px; letter-spacing:1px;\\\" id=\\\"subtitle\\\">' + lang.subtitle + '</small>';";
  html += "  ";
  
  // Update current mode
  html += "  const modeText = currentDisplayMode ? lang.timeDisplay : lang.customText;";
  html += "  document.getElementById('currentMode').textContent = lang.currentMode + modeText;";
  html += "  ";
  
  // Update flash status
  html += "  const flashText = currentFlashMode ? lang.enabled : lang.disabled;";
  html += "  document.getElementById('flashStatus').textContent = lang.flashMessages + flashText;";
  html += "  ";
  
  // Update comrade text if it exists
  html += "  const comradeTextElement = document.getElementById('comradeText');";
  html += "  if (comradeTextElement) {";
  html += "    const textContent = comradeTextElement.textContent.match(/\\\"([^\\\"]*)\\\"/);";
  html += "    if (textContent) {";
  html += "      comradeTextElement.textContent = lang.comradeText + '\\\"' + textContent[1] + '\\\"';";
  html += "    }";
  html += "  }";
  html += "  ";
  
  // Update time display only if it exists (time mode only)
  html += "  const timeElement = document.getElementById('timeDisplay');";
  html += "  if (timeElement) {";
  html += "    const timeValue = timeElement.textContent.split(': ')[1];";
  html += "    timeElement.textContent = lang.moscowTime + timeValue;";
  html += "  }";
  html += "  ";
  
  // Update toggle button
  html += "  const isTimeMode = currentDisplayMode;";
  html += "  document.getElementById('toggleBtn').innerHTML = (isTimeMode ? lang.switchToText : lang.switchToTime);";
  html += "  ";
  
  // Update flash toggle button
  html += "  document.getElementById('flashToggleBtn').innerHTML = (currentFlashMode ? lang.disableFlash : lang.enableFlash);";
  html += "  ";
  
  // Update other elements
  html += "  document.getElementById('messageLabel').textContent = lang.messageLabel;";
  html += "  document.getElementById('customTextInput').placeholder = lang.placeholder;";
  html += "  document.getElementById('setTextBtn').innerHTML = lang.setButton;";
  html += "  ";
  
  // Update language toggle button
  html += "  document.getElementById('langToggle').innerHTML = lang.langButton;";
  
  html += "}";
  html += "";
  html += "function toggleMode() {";
  html += "  fetch('/toggle').then(() => location.reload());";
  html += "}";
  html += "function toggleFlashMessages() {";
  html += "  fetch('/toggleFlashMessage').then(() => location.reload());";
  html += "}";
  html += "function setText() {";
  html += "  const text = document.getElementById('customTextInput').value;";
  html += "  fetch('/settext', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: 'text=' + encodeURIComponent(text) })";
  html += "  .then(() => location.reload());";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  
  return html;
}