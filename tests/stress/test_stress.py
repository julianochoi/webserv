import asyncio
import pytest
import httpx

import config


@pytest.fixture
def event_loop():
    yield asyncio.get_event_loop()


# TODO Disabled for now. Adapt to use siege instead of asyncio
@pytest.mark.skip
@pytest.mark.stress
class TestWebServer:
    @staticmethod
    async def make_request(client, url):
        r: httpx.Response = await client.get(f"http://{url}")
        if config.DEBUG:
            print(r.json())
        return r.status_code

    @pytest.mark.asyncio
    async def test_web_server(self, event_loop):
        async with httpx.AsyncClient() as client:
            tasks = [self.make_request(client, config.SERVER_ADDR) for i in range(config.STRESS_TEST_COUNT)]
            results = await asyncio.gather(*tasks)
            for result in results:
                assert result == 200
