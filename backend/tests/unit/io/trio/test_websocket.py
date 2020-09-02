"""Test the functionality of ventserve.io.trio.websocket."""

import pytest as pt  # type: ignore

import trio

import trio_websocket  # type: ignore

from ventserver.io.trio import websocket


async def client_ping(websocket_props, num_pings=None):
    """Run a WebSocket ping-pong client."""
    async with trio_websocket.open_websocket(
            websocket_props.host, websocket_props.port, '/', use_ssl=False
    ) as connection:
        num_ping = 0
        while True:
            await connection.send_message(b'ping!')
            message = await connection.get_message()
            assert message == b'ping!'
            num_ping += 1
            if num_pings is not None and num_ping >= num_pings:
                return num_ping


async def client_ping_persistently(websocket_props, num_pings=None):
    """Run a WebSocket ping-pong client which persistently tries to connect."""
    while True:
        try:
            return await client_ping(websocket_props, num_pings=num_pings)
        except trio_websocket.ConnectionRejected:
            await trio.sleep(0.1)


async def echo_connection_handler(connection, num_echoes=None):
    """Echo all messages on a connection.."""
    num_echo = 0
    while True:
        try:
            message = await connection.get_message()
            await connection.send_message(message)
            num_echo += 1
            if num_echoes is not None and num_echo >= num_echoes:
                break
        except trio_websocket.ConnectionClosed:
            break


async def echo_request_handler(request, num_echoes=None):
    """Echo all messages with a connection for every request."""
    connection = await request.accept()
    await echo_connection_handler(connection, num_echoes)


@pt.mark.trio
async def test_server(websocket_props, nursery):
    """Test the Server class."""
    server = websocket.Server(echo_request_handler, props=websocket_props)
    assert not server.is_open
    await server.open(nursery=nursery)
    assert server.is_open
    # Check normal client ping
    result = await client_ping(websocket_props, num_pings=10)
    assert result == 10
    # Check that server only opens once
    with pt.raises(RuntimeError):
        await server.open(nursery=nursery)


@pt.mark.trio
async def test_server_close_inside(websocket_props):
    """Test the close functionality of the Server class."""
    server = websocket.Server(echo_request_handler, props=websocket_props)
    with pt.raises(trio_websocket.ConnectionClosed):
        async with trio.open_nursery() as outer_nursery:
            async with trio.open_nursery() as inner_nursery:
                await server.open(nursery=inner_nursery)
                outer_nursery.start_soon(client_ping, websocket_props)
                await client_ping(websocket_props, num_pings=10)
                inner_nursery.cancel_scope.cancel()


@pt.mark.trio
async def test_server_close_outside(websocket_props):
    """Test the close functionality of the Server class."""
    server = websocket.Server(echo_request_handler, props=websocket_props)
    with pt.raises(trio_websocket.ConnectionClosed):
        async with trio.open_nursery() as outer_nursery:
            outer_nursery.start_soon(client_ping_persistently, websocket_props)
            async with trio.open_nursery() as inner_nursery:
                await server.open(nursery=inner_nursery)
                await client_ping(websocket_props, num_pings=10)
                inner_nursery.cancel_scope.cancel()


@pt.mark.trio
async def test_singleconnectionservice(websocket_props, nursery):
    """Test the SingleConnectionService class."""
    service = websocket.SingleConnectionService(
        echo_connection_handler, websocket_props
    )
    assert not service.is_open
    await service.open(nursery=nursery)
    assert service.is_open
    # Check normal client ping
    result = await client_ping(websocket_props, num_pings=10)
    assert result == 10
    # Check that service only opens once
    with pt.raises(RuntimeError):
        await service.open(nursery=nursery)
    # Check normal second client ping
    result = await client_ping(websocket_props, num_pings=20)
    assert result == 20
    # Check rejection of simultaneous second client
    async with trio.open_nursery() as client_nursery:
        client_nursery.start_soon(client_ping, websocket_props)
        await trio.sleep(0.1)
        with pt.raises(trio_websocket.ConnectionRejected):
            await client_ping(websocket_props, num_pings=30)
        client_nursery.cancel_scope.cancel()


@pt.mark.trio
async def test_singleconnectionservice_close(websocket_props):
    """Test the close functionality of the SingleConnectionService class."""
    service = websocket.SingleConnectionService(
        echo_connection_handler, websocket_props
    )
    with pt.raises(trio_websocket.ConnectionClosed):
        async with trio.open_nursery() as outer_nursery:
            async with trio.open_nursery() as inner_nursery:
                await service.open(nursery=inner_nursery)
                outer_nursery.start_soon(client_ping, websocket_props)
                await trio.sleep(0.5)
                inner_nursery.cancel_scope.cancel()


async def client_echo(websocket_props, num_echoes=None):
    """Run a WebSocket echo client."""
    async with trio_websocket.open_websocket(
            websocket_props.host, websocket_props.port, '/', use_ssl=False
    ) as connection:
        num_echo = 0
        while True:
            message = await connection.get_message()
            await connection.send_message(message)
            num_echo += 1
            if num_echoes is not None and num_echo >= num_echoes:
                return num_echo


async def client_echo_persistently(websocket_props, num_echoes=None):
    """Run a WebSocket echo client which persistently tries to connect."""
    while True:
        try:
            return await client_echo(websocket_props, num_echoes=num_echoes)
        except trio_websocket.ConnectionRejected:
            await trio.sleep(0.1)


@pt.mark.trio
async def test_websocketdriver(websocket_props):
    """Test the WebSocketDriver class."""
    driver = websocket.Driver(websocket_props)
    assert not driver.is_open
    async with trio.open_nursery() as nursery:
        nursery.start_soon(client_echo_persistently, websocket_props)
        with pt.raises(RuntimeError):
            await driver.send(b'hello,')
        with pt.raises(RuntimeError):
            await driver.receive()
        await driver.open(nursery=nursery)
        with pt.raises(RuntimeError):
            await driver.open()
        assert driver.is_open
        # Confirm echo
        await driver.send('hello,')
        response = await driver.receive()
        assert response == 'hello,'
        # Confirm echo
        await driver.send('world!')
        response = await driver.receive()
        assert response == 'world!'
        # Confirm close
        await driver.close()
        assert not driver.is_open
        nursery.cancel_scope.cancel()


@pt.mark.trio
async def test_websocketdriver_close(websocket_props):
    """Test the WebSocketDriver class with client connection close."""
    driver = websocket.Driver(websocket_props)
    assert not driver.is_open
    async with trio.open_nursery() as nursery:
        nursery.start_soon(client_echo_persistently, websocket_props, 1)
        await driver.open(nursery=nursery)
        assert driver.is_open
        # Confirm echo
        await driver.send('hello,')
        response = await driver.receive()
        assert response == 'hello,'
        # Confirm connection closed
        await driver.send('world!')
        with pt.raises(BrokenPipeError):
            response = await driver.receive()
        with pt.raises(BrokenPipeError):
            response = await driver.send(b'world!')
        assert driver.is_open
        await driver.close()
        assert not driver.is_open
        nursery.cancel_scope.cancel()
