updateSongs();
let songId = 0;

async function updateSongs()
{
    return await fetch('http://localhost:8080/api/playlist/all')
    .then(response => response.json())
    .then(data =>
        {   
            const songList = document.getElementById('songList');
            songList.innerHTML = "";
            let counter = 1;
            data.forEach(playlist =>{

                playlist.primarySongs.forEach( song =>
                {
                    const songElement = document.createElement('div');
                    songElement.className = "song";

                    const idElement = document.createElement('div');
                    idElement.className = "songId";
                    idElement.textContent = counter;
                    songElement.appendChild(idElement);

                    const buttonElement = document.createElement('button');
                    buttonElement.className = "songButton";
                    buttonElement.onclick = function () 
                    {
                        window.parent.parentCallback(song.id, playlist.id);
                    };
                    songElement.appendChild(buttonElement);

                    const nameElement = document.createElement('div');
                    nameElement.className = "songName";
                    nameElement.textContent = song.name;
                    songElement.appendChild(nameElement);

                    
                    const deleteButton = document.createElement('span');
                    deleteButton.className = "material-icons"
                    deleteButton.innerHTML = "add";
                    deleteButton.onclick = function ()
                    {
                        window.location.href = 'http://localhost:8080/all.html#popup';
                        songId = song.id;
                        updatePopUp(songId);
                        console.log("songId");
                    };
                    songElement.appendChild(deleteButton);

                    songList.appendChild(songElement);
                    counter++;
                }); 
                
        });
    })        
    .catch(error => 
        {
            console.error('Error:', error);
        });
}

async function updatePopUp(songId)
{
    await fetch('http://localhost:8080/api/user/currentUser')
    .then(response => response.text())
    .then(data => {
        fetch('http://localhost:8080/api/user/name/' + data)
        .then(response => response.json())
        .then(data => 
            {
                let counter = 1;
                const songList = document.getElementById('secPlaylists');
                songList.innerHTML = "";
                data.playlists.forEach(playlist => {
                    if(playlist.isPrimary) return;

                    const songElement = document.createElement('div');
                    var bodyData = {
                        songId: songId
                    };
                    songElement.onclick = function ()
                    {
                        fetch("http://localhost:8080/api/playlist/add/secondary/song/" + playlist.id, {
                        method: "POST",
                        headers: {
                            "Content-Type": "application/json" // Adjust the content type if necessary
                        },
                        body: JSON.stringify(bodyData)
                        })
                        .then(response => {
                            // Handle the server response here
                            console.log(data.id);
                            console.log(songId);
                            window.location.href = "#";
                        })
                        .catch(error => {
                            // Handle errors here
                            console.error(error);
                        });
                    };
                    songElement.className = "song";

                    const idElement = document.createElement('div');
                    idElement.className = "songId";
                    idElement.textContent = counter;
                    songElement.appendChild(idElement);

                    const nameElement = document.createElement('div');
                    nameElement.className = "songName";
                    nameElement.textContent = playlist.name;
                    songElement.appendChild(nameElement);
                    
                    songList.appendChild(songElement);

                    counter++;
                });
            })
        .catch(error => 
        {
            console.error('Error:', error);
        });
    })
    .catch(error => 
    {
        console.error('Error:', error);
    });
}