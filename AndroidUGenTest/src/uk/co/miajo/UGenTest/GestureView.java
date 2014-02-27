package uk.co.miajo.UGenTest;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.util.Log;
import android.util.SparseArray;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;

public class GestureView extends View {
	//Create the listener, paint variables, pointer positions array etc
    private GestureListener callbackListener;
    private Paint stringPaint, holePaint;
    private boolean canvasHasBeenDrawn = false;
    private int canvasHeight = 0, canvasWidth = 0, movingPointer;
    public SparseArray<PointF> pointerPositions;
    public boolean slideIsActive = false, velocityTrackerExists = false;
    public float slidePoint = 50.f, bottleneckLength = 0.f;
    public VelocityTracker vTracker = null;
    public int[] stringPositions;
    public int activePointer, strumPointer;
    public float[] fretPositions;
    public float strumPressure;
    
    //Constructor, calls initialise view and listener register function
    public GestureView(Context context, AttributeSet attrs) {
        super(context, attrs);
        //The below arrays are for storing the fret positions on the screen and the frequency values of each fret
        stringPositions = new int[6];
        fretPositions = new float[18];
        initView();
        registerCallbackListener(context);
    	}
    
    private void initView() {
    	//Simply initialises view elements such as paint, and the pointerPosition array
    	pointerPositions = new SparseArray<PointF>();
        stringPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        holePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        stringPaint.setColor(Color.GRAY);
        stringPaint.setStyle(Paint.Style.FILL);
        stringPaint.setStrokeWidth(3);
        stringPaint.setTextSize(50);
        holePaint.setStrokeWidth(3);
        holePaint.setColor(Color.BLACK);
        holePaint.setStyle(Paint.Style.FILL);   
    }
    
    //Registers the listener, if its not implemented, an exception warning is generated
    private void registerCallbackListener(Context context) {
        if (context instanceof GestureListener)
            callbackListener = (GestureListener) context;
        else
            throw new IllegalArgumentException("The activity must implement GestureListener");
    }
    
    private void setStringPositions(){
    	int stringStartPoint = (canvasHeight / 2) - 300;
    	for(int c = 0; c < 6; c++){
    		stringPositions[c] = stringStartPoint;
    		stringStartPoint += 80;
    	}
    }
   
    //Used in order to get the pointer index value of an event
    public int getIndex(MotionEvent event) {
    	  int idx = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
    	  return idx;
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
    	int action = event.getAction() & MotionEvent.ACTION_MASK;
    	int pointerIndex = event.getActionIndex();
    	float windowWidth = getWidth();
    	switch(action) {
    		case MotionEvent.ACTION_DOWN :{
    			int id = event.getPointerId(0);
    			callbackListener.onTouchDown(event, id);
    			PointF f = new PointF();
    		    f.x = event.getX(pointerIndex);
    		    f.y = event.getY(pointerIndex);
    		    pointerPositions.put(id, f);
    		    if(event.getX(getIndex(event)) > ((windowWidth/3) * 2)){
    		    	activePointer = id;
    		    	if(vTracker == null) {
                    	vTracker = VelocityTracker.obtain();
                	}
                	else {
                    	vTracker.clear();
                	}
                	vTracker.addMovement(event);
                	velocityTrackerExists = true;
    		    }
    			break;
    		}
    		case MotionEvent.ACTION_POINTER_DOWN :{
    			int id = event.getPointerId(pointerIndex);
    			callbackListener.onTouchDown(event, id);
    			PointF f = new PointF();
    		    f.x = event.getX(pointerIndex);
    		    f.y = event.getY(pointerIndex);
    		    pointerPositions.put(id, f);
    		    if(event.getX(getIndex(event)) > ((windowWidth/3) * 2)){
    		    	activePointer = id;
    		    	if(vTracker == null) {
                    	vTracker = VelocityTracker.obtain();
                	}
                	else {
                    	vTracker.clear();
                	}
                	vTracker.addMovement(event);
                	velocityTrackerExists = true;
    		    }
    			break;
    		}
    		case MotionEvent.ACTION_MOVE : {
    			for (int size = event.getPointerCount(), i = 0; i < size; i++) {
    		        PointF point = pointerPositions.get(event.getPointerId(i));
    		        if (point != null) {
    		          point.x = event.getX(i);
    		          point.y = event.getY(i);
    		      	  if(point.x < ((windowWidth/3) * 2) - 20 && point.y > canvasHeight - 200){
    		      		  slidePoint = point.x;
    		      		  slideIsActive = true;
    		      		  movingPointer = i;
    		      	  }
    		      	  if(point.x < ((windowWidth/3) * 2) - 20 && point.y <= canvasHeight - 200){
    		      		  slideIsActive = false;
    		      	  }
    		      	  if(point.x > ((windowWidth/3) * 2) && velocityTrackerExists == true){
    		      		  vTracker.addMovement(event);
    		      		  vTracker.computeCurrentVelocity(1000);
    		      		  strumPointer = i;
    		      		  strumPressure = event.getPressure(strumPointer);
    		      	  }
    		      	callbackListener.onMove((int) point.x, (int) point.y, i);
    		        }
    			}
    			break;
    		}
    		case MotionEvent.ACTION_UP :{ 
    			int id = event.getPointerId(0);
    			callbackListener.onTouchUp(event, id);
    			pointerPositions.remove(id);
    			for (int size = event.getPointerCount(), i = 0; i < size; i++) {
    				if(i == movingPointer) slideIsActive = false;
    			}
    			//vTracker.recycle();
    			velocityTrackerExists = false;
    			if(event.getX() < (windowWidth/3) * 2) invalidate();
    			break;
    		}
    		case MotionEvent.ACTION_POINTER_UP :
    		case MotionEvent.ACTION_CANCEL : {
    			int id = event.getPointerId(pointerIndex);
    			callbackListener.onTouchUp(event, id);
    			pointerPositions.remove(id);
    			for (int size = event.getPointerCount(), i = 0; i < size; i++) {
    				if(i == movingPointer) slideIsActive = false;
    			}
    			break;
    		}
    	}
    	invalidate();
    	return true;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        handleCanvasDimensions();
        int startDrawPoint = (canvasHeight / 2) - 300;
        //Draw body hole
        canvas.drawCircle(((canvasWidth/3) * 2) + (canvasWidth/6), canvasHeight/2 - 100, canvasWidth/6, holePaint);
         //Create the point where the strings begin on Y-axis
        //Below loop draws frets and calculates the pitches of each fret note
        for(int f = 1; f<= 18; f++){
        	canvas.drawLine((float) Math.abs((canvasWidth/Math.pow((double) 2, (double) f/12) - canvasWidth)), startDrawPoint - 5, (float) Math.abs((canvasWidth/Math.pow((double) 2, (double) f/12) - canvasWidth)), canvasHeight, stringPaint);
        	fretPositions[f-1] = (float) Math.abs((canvasWidth/Math.pow((double) 2, (double) f/12) - canvasWidth));
        }
        
        if(stringPositions[0] == 0) setStringPositions();
        //Draw divider between neck end and strumming area
        canvas.drawLine((canvasWidth/3) * 2, startDrawPoint - 5, (canvasWidth/3) * 2, (startDrawPoint + (80 * 5)) + 5, holePaint);
        //Draw the strings themselves
        for(int c = 0; c < 6; c++){
        	canvas.drawLine(1, stringPositions[c], canvasWidth, stringPositions[c], stringPaint);
        }
        canvas.drawText("Use slide tool across here", canvasWidth/2 - canvasWidth/3, canvasHeight - 10 , stringPaint);
        //Draw the bottleneck. The size and point varies based on the users slide input
        if(!slideIsActive){
        	bottleneckLength = canvasHeight - 200;
        	canvas.drawRect(slidePoint - 20, bottleneckLength, slidePoint+ 20, canvasHeight, holePaint);
        }
        else{
        	bottleneckLength = (canvasHeight /2) - 310;
        	canvas.drawRect(slidePoint - 20, bottleneckLength, slidePoint + 20, canvasHeight, holePaint);
        }  
    }
    
    private void handleCanvasDimensions() {
    	if(!canvasHasBeenDrawn) {
    		canvasWidth = getWidth();
    		canvasHeight = getHeight();
    		canvasHasBeenDrawn = true;
    	}
    }
}