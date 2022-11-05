package main

import (
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var addr = "0.0.0.0:8080"

var upgrader = websocket.Upgrader{
	// Allow any origin for now
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

var userConnections = make(map[*websocket.Conn]struct{})
var s struct{}

func user(w http.ResponseWriter, r *http.Request) {
	connection, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("User connection error: ", err)
		return
	}

	log.Println("User successfully connected")
	userConnections[connection] = s

	defer connection.Close()
	defer delete(userConnections, connection)

	for {
		_, message, err := connection.ReadMessage()
		if err != nil {
			break
		}
		log.Printf("Received: %s", message)
	}
}

func machine(w http.ResponseWriter, r *http.Request) {
	connection, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("Machine connection error: ", err)
		return
	}

	log.Println("Machine successfully connected")

	defer connection.Close()

	for {
		mt, message, err := connection.ReadMessage()
		if err != nil {
			break
		}

		// Forward byte messages to all users
		if mt == 2 {
			for userConnection := range userConnections {
				userConnection.WriteMessage(mt, message)
			}
		}
	}
}

func main() {
	log.Println("Server starting!")

	http.HandleFunc("/user", user)
	http.HandleFunc("/machine", machine)

	log.Fatal(http.ListenAndServe(addr, nil))
}
