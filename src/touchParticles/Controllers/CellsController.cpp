#include "CellsController.h"

/* Constructor
 ___________________________________________________________ */

CellsController::CellsController()
{
	updateRatio();
	
	_mov.loadMovie("cropped2.mov");
	_mov.play();
}

/* Update
 ___________________________________________________________ */

void CellsController::update(vector <ofxCvTrackedBlob> blobs)
{	
	updateRatio();
	
	for(int i = 0; i < blobs.size(); i++)
	{
		for(int j = 0; j < _cells.size(); j++)
		{
			if (blobs[i].id == _cells[j]->getId()) 
			{
				_cells[j]->update(blobs[i]);
				
				break;
			}
		}
	}
}	

void CellsController::updateRatio()
{
	_ratioX = (float) ofGetWidth() / (float) VIDEO_WIDTH;
	_ratioY = (float) ofGetHeight() / (float) VIDEO_HEIGHT;
}

/* Draw
 ___________________________________________________________ */

void CellsController::draw()
{
	_mov.getTextureReference().bind();
	
	for(int i = 0; i < _cells.size(); i++)
	{
		_cells[i]->draw();
	}
	
	_mov.getTextureReference().unbind();
	
	if(Sensing::getInstance()->debug())
	{
		for(int i = 0; i < _cells.size(); i++)
		{
			_cells[i]->drawDebug();
		}
	}
	
	// draw rectangle grid
	
	
}

/* Blob Events
 ___________________________________________________________ */

void CellsController::blobOn(ofxCvTrackedBlob& blob)
{
	cout << "Blob on" << endl;
	
	if(_cells.size() < NUM_CELLS)
	{
		cout << "Cell created" << endl;
		
		VideoCell * cell = new VideoCell(blob);
		_cells.push_back(cell);
		
		// first rty deleted nums
		if(delCellNums.size() > 0)
		{
			cout << "Got deleted cell num: " << delCellNums[0] << endl;
			cell->setCellNum(delCellNums[0]);
			delCellNums.erase(delCellNums.begin());
			
			return;
		}
		
		// else try all numbers
		for(int i = 0; i < NUM_CELLS; i++)
		{
			bool isThere = false;
			
			cout << "Trying cell num: " << i;
			
			// loop through cells
			for(int j = 0; j < _cells.size(); j++)
			{
				if(i == _cells[j]->getCellNum())
				{
					cout << ", Not Good" << endl;
					isThere = true;
					break;
				}
			}
			
			// if cellnum is not in cells already
			if(!isThere)
			{
				cout << ", GOOD" << endl;
				cell->setCellNum(i);
				break;
			}
		}
	}
}

void CellsController::blobOff(int blobid)
{
	cout << "Blob off" << endl;
	
	for(int i = 0; i < _cells.size(); i++)
	{
		if (_cells[i]->getId() == blobid) 
		{
			delCellNums.push_back(_cells[i]->getCellNum());
			
			cout << "Cell Deleted: " << _cells[i]->getCellNum() << endl;
			
			_cells.erase (_cells.begin()+i);
		}
	}
}

/* Reset
 ___________________________________________________________ */


void CellsController::assignBlobsToCells()
{
	//findOrder();
	
	//setRandom();
	
	_cells.clear();
	delCellNums.clear();
	
	for(int i = 0; i < Sensing::getInstance()->blobTracker.blobs.size(); i++)
	{
		blobOn(Sensing::getInstance()->blobTracker.blobs[i]);
	}
}

void CellsController::findOrder()
{
	vector <SortObject> objects;
	
	for(int i = 0; i < _cells.size(); i++)
	{
		SortObject o;
		o.centroid = _cells[i]->getCentroid();
		o.id = _cells[i]->getId();
		objects.push_back(o);
	}
	
	if(objects.size() != GRID_SIZE)
	{
		cout << "You are trying to order the grid, but there are " << objects.size() << " blobs detected" << endl;
	}
	
	// sort by y
	std::sort(objects.begin(), objects.end(), CellsController::ysorter());
	
	// sort by x
	std::sort(objects.begin(), objects.begin() + 2, CellsController::xsorter());
	std::sort(objects.begin() + 3, objects.begin() + 5, CellsController::xsorter());
	std::sort(objects.begin() + 6, objects.begin() + 8, CellsController::xsorter());
	
	for(int i = 0; i < objects.size(); i++)
	{
		for(int j = 0; j < _cells.size(); j++)
		{
			if(objects[i].id == _cells[j]->getId())
			{
				_cells[j]->setCellNum(i);
			}
		}
	}
}

void CellsController::setRandom()
{
	vector <int> cellNums;
	for(int i = 0; i < _cells.size(); i++)
	{
		cellNums.push_back(i);
	}
	
	for(int i = 0; i < _cells.size(); i++)
	{
		int randomIndex = ofRandom(0, cellNums.size());
		
		_cells[i]->setCellNum(cellNums[randomIndex]);
		
		cellNums.erase(cellNums.begin() + randomIndex);
	}
}


