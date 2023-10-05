const wrapper = document.querySelector(".wrapper"),
musicImg = wrapper.querySelector(".img-area img"),
musicName = wrapper.querySelector(".song-details .name"),
musicArtist = wrapper.querySelector(".song-details .artist"),
playPauseBtn = wrapper.querySelector(".play-pause"),
prevBtn = wrapper.querySelector("#prev"),
nextBtn = wrapper.querySelector("#next"),
mainAudio = wrapper.querySelector("#main-audio"),
progressArea = wrapper.querySelector(".progress-area"),
progressBar = progressArea.querySelector(".progress-bar"),
musicList = wrapper.querySelector(".music-list"),
moreMusicBtn = wrapper.querySelector("#more-music");

let currentSongsList = [];
let currentSongData = null;
let currentPlaylistData = null;

isMusicPaused = true;

window.parentCallback = async function(songId, playlistId) 
{
    if(currentSongData == null || songId != currentSongData.id)
    {
        await fetch('http://localhost:8080/api/playlist/song/' + songId)
        .then(response => response.json())
        .then(data =>
            {
                loadSong(data);
            });
    }

    if(currentPlaylistData == null || playlistId != currentPlaylistData.id)
    {
        await fetch('http://localhost:8080/api/playlist/id/' + playlistId)
        .then(response => response.json())
        .then(data =>
            {
                loadPlaylist(data);
            });
    }
    playMusic();
}

function loadPlaylist(playlistData)
{
    currentPlaylistData = playlistData;
    musicImg.src = "http://localhost:8080/api/playlist/download/image/" + playlistData.id;
    musicArtist.innerText = playlistData.owner.name;
    if(playlistData.isPrimary) currentSongsList = playlistData.primarySongs;
    else currentSongsList = playlistData.secondarySongs;
}

function loadSong(songData)
{
    currentSongData = songData;
    musicName.innerText = songData.name;
    mainAudio.src = "http://localhost:8080/api/playlist/download/song/" + songData.id;
}

function playMusic()
{
  wrapper.classList.add("paused");
  playPauseBtn.querySelector("i").innerText = "pause";
  mainAudio.play();
}

//pause music function
function pauseMusic()
{
  wrapper.classList.remove("paused");
  playPauseBtn.querySelector("i").innerText = "play_arrow";
  mainAudio.pause();
}

function nextSong()
{
  let songIndex = currentSongsList.findIndex( e => e.id == currentSongData.id);
  if(songIndex == currentSongsList.length-1)
  {
    currentSongData = currentSongsList[0];
  }
  else
  {
    currentSongData = currentSongsList[songIndex+1];
  }
  loadSong(currentSongData);
  playMusic();
}

function previousSong()
{
  let songIndex = currentSongsList.findIndex( e => e.id == currentSongData.id);
  if(songIndex == 0)
  {
    currentSongData = currentSongsList[currentSongsList.length-1];
  }
  else
  {
    currentSongData = currentSongsList[songIndex-1];
  }
  loadSong(currentSongData);
  playMusic();
}

nextBtn.addEventListener("click", ()=>{
  nextSong();
});

prevBtn.addEventListener("click", ()=>{
  previousSong();
});

playPauseBtn.addEventListener("click", ()=>{
    const isMusicPlay = wrapper.classList.contains("paused");
    //if isPlayMusic is true then call pauseMusic else call playMusic
    isMusicPlay ? pauseMusic() : playMusic();
});

  mainAudio.addEventListener("timeupdate", (e)=>{
    const currentTime = e.target.currentTime; //getting playing song currentTime
    const duration = e.target.duration; //getting playing song total duration
    let progressWidth = (currentTime / duration) * 100;
    progressBar.style.width = `${progressWidth}%`;
  
    let musicCurrentTime = wrapper.querySelector(".current-time"),
    musicDuartion = wrapper.querySelector(".max-duration");
    mainAudio.addEventListener("loadeddata", ()=>{
      // update song total duration
      let mainAdDuration = mainAudio.duration;
      let totalMin = Math.floor(mainAdDuration / 60);
      let totalSec = Math.floor(mainAdDuration % 60);
      if(totalSec < 10){ //if sec is less than 10 then add 0 before it
        totalSec = `0${totalSec}`;
      }
      musicDuartion.innerText = `${totalMin}:${totalSec}`;
    });
    // update playing song current time
    let currentMin = Math.floor(currentTime / 60);
    let currentSec = Math.floor(currentTime % 60);
    if(currentSec < 10){ //if sec is less than 10 then add 0 before it
      currentSec = `0${currentSec}`;
    }
    musicCurrentTime.innerText = `${currentMin}:${currentSec}`;
});
  
  // update playing song currentTime on according to the progress bar width
progressArea.addEventListener("click", (e)=>{
    let progressWidth = progressArea.clientWidth; //getting width of progress bar
    let clickedOffsetX = e.offsetX; //getting offset x value
    let songDuration = mainAudio.duration; //getting song total duration
    
    mainAudio.currentTime = (clickedOffsetX / progressWidth) * songDuration;
    playMusic(); //calling playMusic function
});

mainAudio.addEventListener("ended", ()=>{
    // we'll do according to the icon means if user has set icon to
    // loop song then we'll repeat the current song and will do accordingly
    nextSong();
    /*
    let getText = repeatBtn.innerText; //getting this tag innerText
    switch(getText){
      case "repeat":
        nextMusic(); //calling nextMusic function
        break;
      case "repeat_one":
        mainAudio.currentTime = 0; //setting audio current time to 0
        loadSong(currentSongData); //calling loadMusic function with argument, in the argument there is a index of current song
        playMusic(); //calling playMusic function
        break;
      case "shuffle":
        let randIndex = Math.floor((Math.random() * allMusic.length) + 1); //genereting random index/numb with max range of array length
        do{
          randIndex = Math.floor((Math.random() * allMusic.length) + 1);
        }while(musicIndex == randIndex); //this loop run until the next random number won't be the same of current musicIndex
        musicIndex = randIndex; //passing randomIndex to musicIndex
        loadMusic(musicIndex);
        playMusic();
        playingSong();
        break;
    }*/
});

