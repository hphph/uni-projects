// Fetch playlist data from the API
updatePlaylists();
async function updatePlaylists()
{
  const playlistsElement = document.getElementById('playlists'); 
  playlistsElement.innerHTML = "";
  getUsername()
  .then(response => response.text())
  .then(data => {
    let username = data;
    document.getElementById('username').textContent = username;
    fetch('http://localhost:8080/api/user/name/' + username)
    .then(response => response.json())
    .then(data => 
      {
        data.playlists.forEach(playlist => 
          {
            const playlistElement = document.createElement('div');
            playlistElement.id = playlist.id;
            playlistElement.className = 'playlist';

            const imageElement = document.createElement('img');
            imageElement.src = "http://localhost:8080/api/playlist/download/image/" + playlist.id;
            imageElement.onclick = function() {location.href='http://localhost:8080/playlist/' + playlistElement.id;};
            imageElement.alt = playlist.name;
            playlistElement.appendChild(imageElement);

            const descriptionElement = document.createElement('div');
            descriptionElement.className = 'description';

            const iconElement = document.createElement('span');
            iconElement.className = 'material-icons';
            iconElement.innerHTML = 'delete';
            iconElement.onclick = function() {deletePlaylist(playlist.id); };
            descriptionElement.appendChild(iconElement);

            const nameElement = document.createElement('p');
            nameElement.textContent = playlist.name;
            descriptionElement.appendChild(nameElement);

            playlistElement.appendChild(descriptionElement);

            playlistsElement.appendChild(playlistElement);
          });

          const playlistElement = document.createElement('i');
          playlistElement.id = "add";
          playlistElement.className = 'material-icons';
          playlistElement.textContent = 'add';
          playlistElement.onclick = function() {window.location.href = 'http://localhost:8080/profile.html#popup';};
          playlistsElement.appendChild(playlistElement);
    })
    .catch(error => {
      console.error('Error:', error);
    });
  });
}

async function getUsername() {
  return await fetch('http://localhost:8080/api/user/currentUser')
    .catch(error => {
      console.error('Error:', error);
    });
}

async function uploadFile(id) {
  const fileInput = document.getElementById("image");
  
  const file = fileInput.files[0];
  const formData = new FormData();
  formData.append("image", file);
  
  await fetch("http://localhost:8080/api/playlist/upload/image/" + id, {
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

async function addPlaylist()
{
  var playlistName = document.getElementById('playlistName').value.trim();
  var isPlaylistPrimary = document.getElementById('isPrimary').checked;
  console.log(playlistName);
  console.log(isPlaylistPrimary);
  if(playlistName)
  {
    var data = {
      name: playlistName,
      isPrimary: isPlaylistPrimary
    };
    await fetch('http://localhost:8080/api/playlist/add', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(data)
    })
    .then(response => response.json())
    .then(async data => {
      console.log(data);
      await uploadFile(data.id);
      // You can update the song list or perform any other actions here
    })
    .catch(error => {
      console.error('Error:', error);
    });
  }

  await updatePlaylists();
}

async function deletePlaylist(playlistId)
{
    await fetch("http://localhost:8080/api/playlist/delete/" + playlistId, 
    {
      method: "DELETE",
      headers: 
      {
        "Content-Type": "application/json" // Adjust the content type if necessary
      },
      // You can include a request body if required
      // body: JSON.stringify({ data: "some data" })
      })
      .then(response => {
        // Handle the server response here
        console.log(response);
      })
      .catch(error => {
        // Handle errors here
        console.error(error);
    });
    await updatePlaylists();
}
