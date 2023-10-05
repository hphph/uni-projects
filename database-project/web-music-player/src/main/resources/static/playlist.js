// Get the URL path
var path = window.location.pathname;

// Remove any trailing slashes
path = path.replace(/\/$/, "");

// Split the path by slashes to get an array of segments
var segments = path.split("/");
var lastSegment = segments[segments.length - 1];

updateSongs();

async function updateSongs()
{
    return await fetch('http://localhost:8080/api/playlist/id/' + lastSegment)
    .then(response => response.json())
    .then(data =>
        {   
            const songList = document.getElementById('songList');
            songList.innerHTML = "";
            data.primarySongs.forEach( song =>
            {
                const songElement = document.createElement('div');
                songElement.className = "song";

                const idElement = document.createElement('div');
                idElement.className = "songId";
                idElement.textContent = song.id;
                songElement.appendChild(idElement);

                const buttonElement = document.createElement('button');
                buttonElement.className = "songButton";
                buttonElement.onclick = function () 
                {
                  window.parent.parentCallback(song.id, lastSegment);
                };
                songElement.appendChild(buttonElement);

                const nameElement = document.createElement('div');
                nameElement.className = "songName";
                nameElement.textContent = song.name;
                songElement.appendChild(nameElement);

                
                const deleteButton = document.createElement('span');
                deleteButton.className = "material-icons"
                deleteButton.innerHTML = "delete";
                deleteButton.onclick = function ()
                {
                    fetch("http://localhost:8080/api/playlist/delete/song/" + song.id, {
                      method: "DELETE",
                      headers: {
                        "Content-Type": "application/json" // Adjust the content type if necessary
                      },
                      // You can include a request body if required
                      // body: JSON.stringify({ data: "some data" })
                    })
                      .then(response => {
                        // Handle the server response here
                        updateSongs();
                      })
                      .catch(error => {
                        // Handle errors here
                        console.error(error);
                      });
                };
                songElement.appendChild(deleteButton);

                songList.appendChild(songElement);
            });

            data.secondarySongs.forEach( song =>
              {
                const songElement = document.createElement('div');
                songElement.className = "song";

                const idElement = document.createElement('div');
                idElement.className = "songId";
                idElement.textContent = song.id;
                songElement.appendChild(idElement);

                const buttonElement = document.createElement('button');
                buttonElement.className = "songButton";
                buttonElement.onclick = function () 
                {
                  window.parent.parentCallback(song.id, lastSegment);
                };
                songElement.appendChild(buttonElement);

                const nameElement = document.createElement('div');
                nameElement.className = "songName";
                nameElement.textContent = song.name;
                songElement.appendChild(nameElement);

                
                const deleteButton = document.createElement('span');
                deleteButton.className = "material-icons"
                deleteButton.innerHTML = "delete";

                var bodyData = {
                  songId: song.id
                };
                deleteButton.onclick = function ()
                {
                    fetch("http://localhost:8080/api/playlist/delete/secondary/song/" + data.id, {
                      method: "DELETE",
                      headers: {
                        "Content-Type": "application/json" // Adjust the content type if necessary
                      },
                      body: JSON.stringify(bodyData)
                    })
                      .then(response => {
                        // Handle the server response here
                        updateSongs();
                      })
                      .catch(error => {
                        // Handle errors here
                        console.error(error);
                      });
                };
                songElement.appendChild(deleteButton);

                songList.appendChild(songElement);
              });
        })
    .catch(error => 
        {
            console.error('Error:', error);
        });
}

async function addSong() {
  var songNameInput = document.getElementById('songNameInput');
  var songName = songNameInput.value.trim();
  if (songName) {
    var data = {
      name: songName
    };

    await fetch('http://localhost:8080/api/playlist/add/song/' + lastSegment, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(data)
    })
    .then(response => response.json())
    .then(data => {
      // Handle the response data as needed
      uploadFile(data.id);
      // You can update the song list or perform any other actions here
    })
    .catch(error => {
      console.error('Error:', error);
    });

    // Clear the input field after adding the song
    songNameInput.value = "";
    await updateSongs();
  }
}

function uploadFile(id) {
  const form = document.getElementById("uploadForm");
  const fileInput = document.getElementById("fileInput");
  
  const file = fileInput.files[0];
  const formData = new FormData();
  formData.append("audio", file);
  
  fetch("http://localhost:8080/api/playlist/upload/song/" + id, {
    method: "POST",
    body: formData
  })
  .then(response => {
    // Handle the server response here
    console.log(response);
  })
  .catch(error => {
    // Handle errors here
    console.error(error);
  });
}







