from fastapi import FastAPI, Query, HTTPException
import httpx

app = FastAPI()

# 1. Endpoint to get lorem text with specified number of words
@app.get("/lorem")
async def get_lorem(words: int = Query(..., gt=0)):
    try:
        async with httpx.AsyncClient() as client:
            # Get 1 paragraph of lorem ipsum from Bacon Ipsum
            response = await client.get("https://baconipsum.com/api/?type=meat-and-filler&paras=1")
            response.raise_for_status()
            paragraph = response.json()[0]
            
            # Trim to desired number of words
            word_list = paragraph.split()
            trimmed = " ".join(word_list[:words])
            return {"lorem": trimmed}
    except httpx.RequestError:
        raise HTTPException(status_code=503, detail="Upstream Bacon Ipsum API unavailable.")
    except Exception as e:
        raise HTTPException(status_code=502, detail=f"Unexpected error: {e}")

# 2. Endpoint to calculate nth Fibonacci number
@app.get("/fibonacci/{n}")
def fibonacci(n: int):
    if n < 0:
        raise HTTPException(status_code=400, detail="Value must be non-negative.")
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return {"fibonacci": a}

# 3. Simple Hello World endpoint
@app.get("/hello")
def hello():
    return {"message": "Hello, world!"}
# Run
# $ uvicorn main:app --reload
# Or
# $ uvicorn py_src.lab_five.webserv:app --reload --host 192.168.31.103 --port 8000