Imports System
Imports Tinkerforge

Module ExampleInput
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your IO-4 Bricklet 2.0

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim io As New BrickletIO4V2(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current value
        Dim value As Boolean() = io.GetValue()

        Console.WriteLine("Channel 0: " + value(0).ToString())
        Console.WriteLine("Channel 1: " + value(1).ToString())
        Console.WriteLine("Channel 2: " + value(2).ToString())
        Console.WriteLine("Channel 3: " + value(3).ToString())

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
