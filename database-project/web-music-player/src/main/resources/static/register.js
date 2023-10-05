async function register()
{
    var registerInput = document.getElementById('registerInput');
    var nickToRegister = registerInput.value.trim();
    if(nickToRegister)
    {
        var data = {
            name: nickToRegister
        };

        await fetch('http://localhost:8080/api/user/register', 
        {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
              },
            body: JSON.stringify(data)
        })
        .then(response => response)
        .then(data =>
        {
            console.log(data);
        })
        .catch(error => 
        {
            console.error(data);
            console.error('Error:', error);
        });
    }
}

async function onRegisterClick()
{
    await register();
    window.location.href = "http://localhost:8080/";
}